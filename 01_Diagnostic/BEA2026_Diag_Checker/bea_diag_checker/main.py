"""Application entry point."""

from __future__ import annotations

import sys
import tkinter as tk
from pathlib import Path

if __name__ == "__main__":
    # Support running directly from IDE
    pkg_dir = Path(__file__).resolve().parent.parent
    if str(pkg_dir) not in sys.path:
        sys.path.insert(0, str(pkg_dir))
    from bea_diag_checker.ui.main_window import BEADiagChecker
else:
    from .ui.main_window import BEADiagChecker

def main() -> None:
    """Start the BEA Diag Checker window."""
    try:
        root = tk.Tk()
    except tk.TclError as exc:
        raise SystemExit(
            "Unable to start the GUI. Install Python 3.12.13.1 with Tcl/Tk support."
        ) from exc
    BEADiagChecker(root)
    root.mainloop()

__all__ = ["main"]

if __name__ == "__main__":
    main()
