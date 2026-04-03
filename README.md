# Shop Stock Controller (C++ WebAssembly)

## Project Overview
The **Shop Stock Controller** is a console-based Inventory Management system developed in C++ following strict Object-Oriented Programming (OOP) paradigms. It empowers retail shop owners to efficiently manage their stock—adding items, purchasing, selling, generating low stock alerts, and evaluating overall inventory value.

This project goes a step further by utilizing **Emscripten** to transpile the C++ execution logic perfectly into **WebAssembly (WASM)**, allowing seamless functionality within any modern web browser without server-side compute. 

*Mandatory Rules Followed:*
- C++ execution logic strictly retained using standard encapsulation and native C++ abstractions. 
- Input parsing uses Emscripten's natively integrated `EM_JS` combined with `ASYNCIFY` to pause the WASM execution, avoiding browser thread blocking while rendering a clean input box, simulating the terminal interface perfectly instead of depending on ugly popup `prompt()` dialogues!

## Features
- **Add Product:** Add unique products into the inventory (enforces ID uniqueness).
- **Purchase Stock:** Increment product availability easily.
- **Sell Stock:** Dispatch products, blocking operations and raising errors if insufficient quantity exists.
- **Search System:** Search through inventory by perfect `ProductID` match or `Name` string keyword.
- **Delete Product:** Remove discontinued products securely using a confirmation prompt.
- **File Handling Persistence:** Loads data on program startup (constructor) and secures state to text file upon exit/changes.
- **Reports:** Fast extraction of *Total Inventory Value* and *Low-stock Reports*.

## Compilation & Execution Using Emscripten (`build.bat`)

Since standard `std::cin` visually pauses execution using a blocking web popup, your `main.cpp` code overrides this utilizing `ASYNCIFY` to smoothly link the HTML form input to your backend C++ code.

**How to Compile & Run (Automated):**
1. Ensure your terminal is at the project root (`C:\Users\Administrator\Desktop\da2c`).
2. Simply double click or run:
   ```cmd
   build.bat
   ```
   *This automated script activates your emsdk, correctly compiles `index.js` featuring all required ASYNCIFY and _malloc exports, and automatically starts your local Python server.*
3. Access the application in your browser by visiting: `http://localhost:8080/index.html`. You will now be able to type cleanly into the terminal input box!
