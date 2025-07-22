R Interface to the ncurses library for terminal user interfaces
======

![GPLv3](http://www.gnu.org/graphics/gplv3-88x31.png)
[GNU General Public License, GPLv3](http://www.gnu.org/copyleft/gpl.html)


# Example Usage

```r
text_editor <- function() {
  # Initialize screen
  crtInitScreen()
  
  # Get window dimensions
  dims <- crtGetWinMax()
  height <- dims[1]
  width <- dims[2]
  
  # Draw header
  crtMove(y = 0, x = 0)
  crtPutText(txt = "Simple Text Editor", col = "blue")
  crtMove(y = 1, x = 0)
  crtPutText(txt = paste(rep("-", width - 1), collapse = ""), col = "blue")
  
  # Draw footer
  crtMove(y = height - 2, x = 0)
  crtPutText(txt = paste(rep("-", width - 1), collapse = ""), col = "blue")
  crtMove(y = height - 1, x = 0)
  crtPutText(txt = "Press ESC to exit", col = "blue")
  
  # Initial cursor position for text area
  row <- 3
  col <- 0
  crtMove(y = row, x = col)
  crtRefresh()
  
  # Main loop
  running <- TRUE
  while(running) {
    key <- crtGetKey()
    
    if (key == KEY_ESC) {
      running <- FALSE
    } else if (key == KEY_ENTER) {
      row <- row + 1
      col <- 0
      crtMove(y = row, x = col)
    } else if (key >= 32 && key <= 126) {  # Printable ASCII
      crtPutText(txt = rawToChar(as.raw(key)))
      col <- col + 1
      if (col >= width - 1) {
        col <- 0
        row <- row + 1
      }
    }
    
    crtRefresh()
  }
  
  # Clean up
  crtDoneScreen()
}
library(TerminalR)
text_editor()
```