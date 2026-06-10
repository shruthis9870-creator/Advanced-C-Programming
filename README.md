# Advanced-C-Programming
Menu-driven 2D Graphics Editor in C using a 2D character array as the drawing canvas

## Compile and run

```sh
gcc graphics_editor.c -o graphics_editor
./graphics_editor
```

On Windows PowerShell, run:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
gcc graphics_editor.c -o graphics_editor.exe
.\graphics_editor.exe
```

If `gcc` is not recognized, run the first PowerShell line again or restart
VS Code and reopen the terminal.
## Project Description

This mini project is a menu-driven 2D Graphics Editor written in the C
programming language. It uses a two-dimensional character array as the drawing
canvas. The blank area of the canvas is filled with underscores (`_`), and the
drawn objects are represented with asterisks (`*`).

The program allows the user to add, delete, modify, list, clear, and display
graphical objects. It supports four basic shapes: line, rectangle, circle, and
triangle. Each object is stored with an ID and its required coordinate values,
so the user can later select that object for deletion or modification.

The canvas size is fixed at 25 rows and 60 columns. Input validation is used to
make sure all coordinates stay within the canvas boundary. Circle radius values
are also checked so that a circle fits inside the canvas. This keeps the stored
object data consistent with the displayed picture.

The project demonstrates important C programming concepts such as arrays,
structures, enums, functions, loops, conditional statements, menu handling, and
basic input validation.
