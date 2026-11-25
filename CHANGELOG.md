# Changelog

Format: [Keep a Changelog](https://keepachangelog.com/), versions: [SemVer](https://semver.org/).
Before 1.0, minor versions may break the API.

## [Unreleased]
### Added
- `TouchEvent::speed`: swipe speed in px/s (SWIPE_UP/DOWN).
- Meaning-based colors in `ui::color`: `EDITABLE`, `ACTIVE`, `LIMIT`, `TEMPORARY`, `REQUIRED`, `NEUTRAL` (Airbus MCDU convention, README "Colors").
### Changed
- `AIRBUS_YELLOW` calibrated to 240,230,0 (pure yellow looks greenish on the M5Tough panel).

## [0.1.0]
### Added
- DisplayManager (pages, swipe navigation, touch gestures, screensaver, `on_page_settled` callback).
- `IScreenPage` / `IScreensaver` interfaces, `TouchEvent`.
- Drawers: Ecam, PageRowDrawer, TableDrawers, AnnunciationBoard, TouchButton, IconLoader, Modal.
- Screensavers: Shapes, Shapes2, Meltable, InProgress.
- Embedded Honeywell MCDU VLW fonts, ECAM font slots with fallback.
