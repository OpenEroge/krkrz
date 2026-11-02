//---------------------------------------------------------------------------
// Engine extension hooks (see EngineHooks.h)
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#include "EngineHooks.h"

namespace {
// Function-local statics: hooks may be registered during static
// initialisation of other translation units.
template <typename T> std::vector<T> &Hooks()
{
	static std::vector<T> hooks;
	return hooks;
}
}

void TVPAddStorageRedirectHook(tTVPStorageRedirectHook hook) { Hooks<tTVPStorageRedirectHook>().push_back(hook); }
void TVPAddStorageLeafHook(tTVPStorageLeafHook hook) { Hooks<tTVPStorageLeafHook>().push_back(hook); }
void TVPAddStorageFallbackHook(tTVPStorageFallbackHook hook) { Hooks<tTVPStorageFallbackHook>().push_back(hook); }
void TVPAddStorageEventHook(tTVPStorageEventHook hook) { Hooks<tTVPStorageEventHook>().push_back(hook); }

tTVPStorageRedirect TVPRunStorageRedirectHooks(const ttstr &requested, ttstr &out)
{
	for (auto hook : Hooks<tTVPStorageRedirectHook>()) {
		tTVPStorageRedirect r = hook(requested, out);
		if (r != tTVPStorageRedirect::None) return r;
	}
	return tTVPStorageRedirect::None;
}

ttstr TVPRunStorageLeafHooks(const ttstr &leaf)
{
	for (auto hook : Hooks<tTVPStorageLeafHook>()) {
		ttstr placed = hook(leaf);
		if (!placed.IsEmpty()) return placed;
	}
	return ttstr();
}

void TVPRunStorageFallbackHooks(const ttstr &leaf, std::vector<ttstr> &alternatives)
{
	for (auto hook : Hooks<tTVPStorageFallbackHook>()) hook(leaf, alternatives);
}

void TVPRunStorageEventHooks(const char *kind, const ttstr &path)
{
	for (auto hook : Hooks<tTVPStorageEventHook>()) hook(kind, path);
}

void TVPAddGraphicSniffer(const tTVPGraphicSniffer &sniffer) { Hooks<tTVPGraphicSniffer>().push_back(sniffer); }

bool TVPRunGraphicSniffers(tTVPGraphicSizeCallback sizecallback, tTVPGraphicScanLineCallback scanlinecallback,
	void *callbackdata, iTJSBinaryStream *src, tTVPGraphicLoadMode mode)
{
	for (const auto &s : Hooks<tTVPGraphicSniffer>())
		if (s.Load && s.Load(sizecallback, scanlinecallback, callbackdata, src, mode)) return true;
	return false;
}

bool TVPRunGraphicHeaderSniffers(iTJSBinaryStream *src, iTJSDispatch2 **dic)
{
	for (const auto &s : Hooks<tTVPGraphicSniffer>())
		if (s.LoadHeader && s.LoadHeader(src, dic)) return true;
	return false;
}

void TVPAddPresentHook(tTVPPresentHook hook) { Hooks<tTVPPresentHook>().push_back(hook); }
void TVPAddDispatchHook(tTVPDispatchHook hook) { Hooks<tTVPDispatchHook>().push_back(hook); }
bool TVPHasPresentHooks() { return !Hooks<tTVPPresentHook>().empty(); }

void TVPRunPresentHooks(tjs_uint64 swap_ns, tjs_uint64 show_ns)
{
	for (auto hook : Hooks<tTVPPresentHook>()) hook(swap_ns, show_ns);
}

void TVPRunDispatchHooks()
{
	for (auto hook : Hooks<tTVPDispatchHook>()) hook();
}
