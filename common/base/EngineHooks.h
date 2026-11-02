//---------------------------------------------------------------------------
// Engine extension hooks
//
// Code linked statically into the executable (compatibility modules, game
// specific glue) registers callbacks here instead of being called by name
// from engine sources. The engine never knows who is listening.
//
// All registration functions may be called at any time from the main thread;
// in practice modules register during static initialisation or from a
// tTVPAtInstallClass callback. Hooks run in registration order.
//---------------------------------------------------------------------------
#ifndef EngineHooksH
#define EngineHooksH

#include "tjsCommHead.h"
#include "GraphicsLoaderIntf.h"
#include <vector>

//---------------------------------------------------------------------------
// Storage name resolution (TVPGetPlacedPath)
//---------------------------------------------------------------------------
enum class tTVPStorageRedirect {
	None,    //!< leave the request alone
	Placed,  //!< out is the final placed path; it is not cached
	Renamed, //!< look up out instead of the requested name
};
//! Runs first, for every request and before the auto path cache, so the
//! answer may change between calls (e.g. a language-dependent redirect).
typedef tTVPStorageRedirect (*tTVPStorageRedirectHook)(const ttstr &requested, ttstr &out);
//! Runs before the auto path table is searched for a leaf name (bare names
//! and paths that do not exist). Returns a placed path or an empty string.
//! A non-empty result overrides every auto path and is cached.
typedef ttstr (*tTVPStorageLeafHook)(const ttstr &leaf);
//! Runs when a leaf was not found anywhere. Appends alternative leaf names;
//! each is tried next to the requested path, then in the auto path table.
typedef void (*tTVPStorageFallbackHook)(const ttstr &leaf, std::vector<ttstr> &alternatives);
//! Diagnostic events from the storage system: "scan", "listing_hit",
//! "rebuild", "write", "clear". Called with the storage lock held.
typedef void (*tTVPStorageEventHook)(const char *kind, const ttstr &path);

void TVPAddStorageRedirectHook(tTVPStorageRedirectHook hook);
void TVPAddStorageLeafHook(tTVPStorageLeafHook hook);
void TVPAddStorageFallbackHook(tTVPStorageFallbackHook hook);
void TVPAddStorageEventHook(tTVPStorageEventHook hook);

// used by StorageIntf.cpp
tTVPStorageRedirect TVPRunStorageRedirectHooks(const ttstr &requested, ttstr &out);
ttstr TVPRunStorageLeafHooks(const ttstr &leaf);
void TVPRunStorageFallbackHooks(const ttstr &leaf, std::vector<ttstr> &alternatives);
void TVPRunStorageEventHooks(const char *kind, const ttstr &path);

//---------------------------------------------------------------------------
// Image content sniffing
//
// Some data ships images in a format other than its extension says (e.g.
// WebP files named *.png). A sniffer looks at the stream and either decodes
// it completely (returns true) or leaves the stream position untouched and
// returns false so the regular loader continues. Currently consulted by the
// PNG loader.
//---------------------------------------------------------------------------
struct tTVPGraphicSniffer {
	bool (*Load)(tTVPGraphicSizeCallback sizecallback, tTVPGraphicScanLineCallback scanlinecallback,
		void *callbackdata, iTJSBinaryStream *src, tTVPGraphicLoadMode mode);
	bool (*LoadHeader)(iTJSBinaryStream *src, iTJSDispatch2 **dic);
};
void TVPAddGraphicSniffer(const tTVPGraphicSniffer &sniffer);
bool TVPRunGraphicSniffers(tTVPGraphicSizeCallback sizecallback, tTVPGraphicScanLineCallback scanlinecallback,
	void *callbackdata, iTJSBinaryStream *src, tTVPGraphicLoadMode mode);
bool TVPRunGraphicHeaderSniffers(iTJSBinaryStream *src, iTJSDispatch2 **dic);

//---------------------------------------------------------------------------
// Frame timing
//---------------------------------------------------------------------------
//! Called by the draw device after a frame was presented (buffer swap
//! returned) with the GL context current. swap_ns: time spent in the swap,
//! show_ns: time spent in the whole Show(). Only measured while at least one
//! hook is registered.
typedef void (*tTVPPresentHook)(tjs_uint64 swap_ns, tjs_uint64 show_ns);
//! Called at the start of every main loop event dispatch.
typedef void (*tTVPDispatchHook)();

void TVPAddPresentHook(tTVPPresentHook hook);
void TVPAddDispatchHook(tTVPDispatchHook hook);
bool TVPHasPresentHooks();
void TVPRunPresentHooks(tjs_uint64 swap_ns, tjs_uint64 show_ns);
void TVPRunDispatchHooks();

#endif
