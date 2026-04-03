#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace std;

// ---------------------------------------------------------
// Seamless WebAssembly Terminal Input handler
// ---------------------------------------------------------
#ifdef __EMSCRIPTEN__
EM_JS(void, getConsoleInputJS, (char* buf, int maxLen), {
    return Asyncify.handleSleep(function(wakeUp) {
        var inputEl = document.getElementById("cmdInput");
        if (inputEl) {
            inputEl.disabled = false;
            inputEl.placeholder = "⌨ Type your input here and press Enter...";
            inputEl.focus();
            
            var enterHandler = function(e) {
                if (e.key === 'Enter') {
                    var val = inputEl.value;
                    inputEl.removeEventListener("keydown", enterHandler);
                    inputEl.value = "";
                    inputEl.disabled = true;
                    
                    var outEl = document.getElementById('output');
                    if (outEl) {
                        outEl.value += val + "\n";
                        outEl.scrollTop = outEl.scrollHeight;
                    }

                    // Directly write to WASM memory without needing exported Emscripten functions!
                    var encoder = new TextEncoder();
                    var bytes = encoder.encode(val);
                    var len = bytes.length;
                    if (len >= maxLen) len = maxLen - 1;
                    for (var i = 0; i < len; i++) {
                        HEAPU8[buf + i] = bytes[i];
                    }
                    HEAPU8[buf + len] = 0; // null terminator
                    
                    wakeUp();
                }
            };
            inputEl.addEventListener("keydown", enterHandler);
        } else {
            var val = window.prompt("Input:") || "";
            var encoder = new TextEncoder();
            var bytes = encoder.encode(val);
            var len = bytes.length;
            if (len >= maxLen) len = maxLen - 1;
            for (var i = 0; i < len; i++) {
                HEAPU8[buf + i] = bytes[i];
            }
            HEAPU8[buf + len] = 0;
            wakeUp();
        }
    });
});
#endif

// Universal input reading function adapting based on compiler
string getConsoleInput() {
    cout << endl; // Force complete line flush in Emscripten terminal before sleeping
#ifdef __EMSCRIPTEN__
    char buf[1024] = {0};
    getConsoleInputJS(buf, 1024);
    string result(buf);
    return result;
#else
    string result;
    cin >> ws; // clear leading whitespaces
    getline(cin, result);
    return result;
#endif
}


// ---------------------------------------------------------
// Mandatory OOP Requirement 1: Product Class
// ---------------------------------------------------------
class Product {
private:
    string productID;
    string name;
    double price;
    int quantity;

public:
    Product(string id = "", string n = "", double p = 0.0, int q = 0) {
        productID = id;
        name = n;
        price = p;
        quantity = q;
    }

    string getProductID() const { return productID; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void addQuantity(int amount) {
        quantity += amount;
    }

    bool reduceQuantity(int amount) {
        if (quantity >= amount) {
            quantity -= amount;
            return true;
        }
        return false;
    }

    void display() const {
        cout << left << setw(15) << productID 
             << setw(25) << name 
             << "$" << setw(14) << fixed << setprecision(2) << price 
             << setw(10) << quantity << endl;
    }

    string serialize() const {
        return productID + "," + name + "," + to_string(price) + "," + to_string(quantity);
    }
};

// ---------------------------------------------------------
// Mandatory OOP Requirement 2: Inventory Class
// ---------------------------------------------------------
class Inventory {
private:
    vector<Product> products;
    string filename;

    void parseLine(const string& line, string& id, string& name, double& price, int& qty) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);

        if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
            id = line.substr(0, pos1);
            name = line.substr(pos1 + 1, pos2 - pos1 - 1);
            try {
                price = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
                qty = stoi(line.substr(pos3 + 1));
            } catch (...) {
                price = 0.0;
                qty = 0;
            }
        }
    }

    int findProductIndex(const string& id) const {
        for (size_t i = 0; i < products.size(); i++) {
            if (products[i].getProductID() == id) {
                return i;
            }
        }
        return -1;
    }

    void printHeader() const {
        cout << string(65, '-') << "\n";
        cout << left << setw(15) << "Product ID" 
             << setw(25) << "Name" 
             << setw(15) << "Price" 
             << setw(10) << "Quantity" << "\n";
        cout << string(65, '-') << "\n";
    }

public:
    Inventory(string file = "inventory.txt") {
        filename = file;
        loadFromFile();
    }

    ~Inventory() {
        saveToFile();
    }

    void addProduct() {
        string id, name;
        double price;
        int qty;

        cout << "\n--- Add New Product ---\n";
        cout << "Enter Product ID: ";
        id = getConsoleInput();
        if (id.empty()) return;

        if (findProductIndex(id) != -1) {
            cout << ">> Error: Product ID already exists!\n";
            return;
        }

        cout << "Enter Product Name: ";
        name = getConsoleInput();
        
        cout << "Enter Price: $";
        string priceInput = getConsoleInput();
        try { price = stod(priceInput); } catch (...) { price = -1; }
        
        cout << "Enter Initial Quantity: ";
        string qtyInput = getConsoleInput();
        try { qty = stoi(qtyInput); } catch (...) { qty = -1; }

        if (price < 0 || qty < 0) {
            cout << ">> Error: Price and Quantity cannot be negative/invalid.\n";
            return;
        }

        Product newProduct(id, name, price, qty);
        products.push_back(newProduct);
        cout << ">> Product added successfully.\n";
        saveToFile();
    }

    void updateStock(bool isPurchase) {
        string id;
        int amount;

        cout << "\n--- " << (isPurchase ? "Purchase" : "Sell") << " Stock ---\n";
        cout << "Enter Product ID: ";
        id = getConsoleInput();

        int index = findProductIndex(id);
        if (index == -1) {
            cout << ">> Error: Product not found!\n";
            return;
        }

        cout << "Enter quantity to " << (isPurchase ? "add" : "sell") << ": ";
        string amtInput = getConsoleInput();
        try { amount = stoi(amtInput); } catch (...) { amount = -1; }

        if (amount <= 0) {
            cout << ">> Error: Quantity must be greater than zero.\n";
            return;
        }

        if (isPurchase) {
            products[index].addQuantity(amount);
            cout << ">> Stock updated successfully. New quantity: " << products[index].getQuantity() << "\n";
        } else {
            if (products[index].reduceQuantity(amount)) {
                cout << ">> Sale successful. Remaining quantity: " << products[index].getQuantity() << "\n";
            } else {
                cout << ">> Error: Insufficient stock (" << products[index].getQuantity() << " available).\n";
            }
        }
        saveToFile();
    }

    void searchProduct() const {
        string query;
        cout << "\n--- Search Product ---\n";
        cout << "Enter Product ID or Name keyword: ";
        query = getConsoleInput();

        bool found = false;
        
        for (size_t i = 0; i < products.size(); i++) {
            if (products[i].getProductID() == query || products[i].getName().find(query) != string::npos) {
                if (!found) { printHeader(); }
                products[i].display();
                found = true;
            }
        }

        if (!found) {
            cout << ">> No matching products found.\n";
        } else {
            cout << string(65, '-') << "\n";
        }
    }

    void deleteProduct() {
        string id;
        cout << "\n--- Delete Product ---\n";
        cout << "Enter Product ID to delete: ";
        id = getConsoleInput();

        int index = findProductIndex(id);
        if (index == -1) {
            cout << ">> Error: Product not found!\n";
            return;
        }

        cout << "Are you sure you want to delete '" << products[index].getName() << "'? (y/n): ";
        string confirm = getConsoleInput();

        if (confirm == "y" || confirm == "Y") {
            products.erase(products.begin() + index);
            cout << ">> Product deleted successfully.\n";
            saveToFile();
        } else {
            cout << ">> Deletion cancelled.\n";
        }
    }

    void generateLowStockReport(int threshold = 5) const {
        cout << "\n--- Low Stock Report (Threshold: " << threshold << ") ---\n";
        bool found = false;
        
        for (const auto& product : products) {
            if (product.getQuantity() < threshold) {
                if (!found) { printHeader(); }
                product.display();
                found = true;
            }
        }

        if (!found) {
            cout << ">> All products have sufficient stock.\n";
        } else {
            cout << string(65, '-') << "\n";
        }
    }

    void displayTotalValue() const {
        double total = 0.0;
        for (const auto& product : products) {
            total += product.getPrice() * product.getQuantity();
        }
        cout << "\n============================================\n";
        cout << " Total Inventory Value: $" << fixed << setprecision(2) << total << "\n";
        cout << "============================================\n";
    }

    void displayAllProducts() const {
        if (products.empty()) {
            cout << "\n>> Inventory is empty.\n";
            return;
        }
        cout << "\n--- All Products contained in Inventory ---\n";
        printHeader();
        for (const auto& product : products) {
            product.display();
        }
        cout << string(65, '-') << "\n";
    }

    void saveToFile() const {
        ofstream outFile(filename, ios::trunc);
        if (outFile.is_open()) {
            for (const auto& product : products) {
                outFile << product.serialize() << "\n";
            }
            outFile.close();
        }
    }

    void loadFromFile() {
        ifstream inFile(filename);
        if (inFile.is_open()) {
            string line, id, name;
            double price;
            int qty;
            while (getline(inFile, line)) {
                if (line.empty()) continue;
                parseLine(line, id, name, price, qty);
                products.push_back(Product(id, name, price, qty));
            }
            inFile.close();
        }
    }
};

int main() {
    Inventory shopInventory;
    int choice = 0;

    do {
        cout << "\n===============================\n";
        cout << "    SHOP STOCK CONTROLLER      \n";
        cout << "===============================\n";
        cout << "1. Add New Product\n";
        cout << "2. Purchase Stock (Increase)\n";
        cout << "3. Sell Stock (Decrease)\n";
        cout << "4. Search Product\n";
        cout << "5. Delete Product\n";
        cout << "6. View All Products\n";
        cout << "7. Low-Stock Report\n";
        cout << "8. Total Inventory Value\n";
        cout << "9. Exit System\n";
        cout << "-------------------------------\n";
        cout << "Enter your choice: ";
        
        string choiceInput = getConsoleInput();
        try {
            choice = stoi(choiceInput);
        } catch (...) {
            choice = 0;
        }

        switch (choice) {
            case 1: shopInventory.addProduct(); break;
            case 2: shopInventory.updateStock(true); break;
            case 3: shopInventory.updateStock(false); break;
            case 4: shopInventory.searchProduct(); break;
            case 5: shopInventory.deleteProduct(); break;
            case 6: shopInventory.displayAllProducts(); break;
            case 7: shopInventory.generateLowStockReport(10); break;
            case 8: shopInventory.displayTotalValue(); break;
            case 9: cout << "\n>> Powering off... Data Saved.\n"; break;
            default: cout << ">> Invalid choice. Choose between 1-9.\n";
        }
    } while (choice != 9);

    return 0;
}
