# first-storm/krkrz

A hard fork of [wamsoft/krkrz](https://github.com/wamsoft/krkrz) at `7d91ba5`
(tag `upstream-7d91ba5`), used as the engine of lllj-krkrz, a native Linux
port of Kirikiri Z games.

The fork stays game-agnostic. Changes fall into two groups:

* fixes that are right for every game (lock ordering in the sound event
  thread and the pooled allocator, draw-thread splitting, case-insensitive
  paths, VideoOverlay period events / segment loops, incremental auto-path
  index, `SDLOGLDrawDevice.recreate()`, a debugger scope-key cache);
* extension points for code linked into the executable
  (`common/base/EngineHooks.h`: storage resolution, image sniffing, present
  and dispatch callbacks; `TVPExternalMoviePlayerFactory` in `MoviePlayer.h`).

`git log upstream-7d91ba5..` lists every change with its reason. Upstream fixes
are taken by cherry-pick, not by merging.

New options: `-drawthreadchunk=N` (default 10000, 0 = upstream behaviour),
`-autopath_incremental=no` (upstream auto-path rebuilds).
