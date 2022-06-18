/**
----------------ASSIGNMENT 2--------------------------
* COURSE: DBS311
* SECTION: NCC
* Pedro do Carmo
*/



#include <iostream>
#include <iomanip>
#include <occi.h>
using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;



struct ShoppingCart {
	int product_id;
	double price;
	int quantity;
};

/**
* Compare the input from the user to the customer table from the database
*
* @param conn Connection allows the connetion to the database
* @param customerId int used to compare the customer_id from the database
*
* @return number that represents with the customer was found or not in the database. 1 the customer was found, 0 the customer wasn't found.
*/
int customerLogin(Connection* conn, int customerId) {
	Statement* findCust = conn->createStatement();
	findCust->setSQL("begin find_customer(:1, :2); end;");
	findCust->setNumber(1, customerId);
	findCust->registerOutParam(2, OCCIINT);

	findCust->execute();

	int foundCust = findCust->getInt(2);

	conn->terminateStatement(findCust);
	return foundCust;

}

/**
* Compare the input from the user to the product table from the database
*
* @param conn Connection allows the connetion to the database
* @param productId int used to compare the product_id from the database
*
* @return number that represents with the product was found or not in the database. retunr 1 means it was found, 0 means it wasn't found.
*/
double findProduct(Connection* conn, int product_id) {
	double price;
	Statement* findProduct = conn->createStatement();
	findProduct->setSQL("begin find_product(:1, :2); end;");
	findProduct->setNumber(1, product_id);
	findProduct->registerOutParam(2, OCCIDOUBLE);
	findProduct->execute();

	price = findProduct->getDouble(2);
	conn->terminateStatement(findProduct);

	return price;
}

/**
* Add the products found in the shopping cart
*
* @param conn Connection allows the connetion to the database
* @param cart ShoppingCart used to store the values from the product found to the cart attributes
*
* @return int that represents the number of products stored in the cart param.
*/
int addToCart(Connection* conn, struct ShoppingCart cart[]) {
	int productId;
	double price;
	int qty = 0;
	int count = 0;



	// Flag that means if the customer wants to add more product
	int add = 1;

	// It is possible to add up to 5 products
	for (int i = 0; i <= 5; i++) {
		// if is not adding the first product
		if(add == 1){
			std::cout << "Enter the product ID: ";
			std::cin >> productId;
			price = findProduct(conn, productId);

			//if product not found in the database
			while (price == 0) {
				std::cout << "The product does not exists. Try again..." << std::endl;
				std::cout << "Enter the product ID: ";
				std::cin >> productId;
				price = findProduct(conn, productId);
			}

			std::cout << "Product Price: " << price << std::endl;
			std::cout << "Enter the product Quantity : ";
			std::cin >> qty;
			
			cart[i].product_id = productId;
			cart[i].price = price;
			cart[i].quantity = qty;
			
			count++;

			do {
				std::cout << "Enter 1 to add more products or 0 to checkout: ";
				std::cin >> add;
				if (add != 1 && add != 0) {
					std::cout << "Wrong input. Try again..." << std::endl;
				}
			} while (add != 1 && add != 0);
			if (add == 0) {
				//Allows to exit the for loop
				i = 6;
			}
		}

		
	}
	return count;
}

/**
* Displays the products stored in the shopping cart, and calculates the total amount.
*
* @param cart ShoppingCart is the user shopping cart with the products stored in it.
* @param productCount int is the number of products stored in the user's shopping cart
*
*
*/
void displayProducts(struct ShoppingCart cart[], int productCount) {
	double total = 0;
	
	//Loop through all the cart elements(products), displaying each attribute of the element.
	for (int i = 0; i < productCount; i++) {
		std::cout << "---Item" << (i + 1) << std::endl;
		std::cout << "Product ID: " << cart[i].product_id << std::endl;
		std::cout << "Price: " << cart[i].price << std::endl;
		std::cout << "Quantity: " << cart[i].quantity << std::endl;

		//calculate the total. Sum (price * quantity) from all elements. 
		total += (cart[i].price * cart[i].quantity);
	}

	std::cout << "----------------------------------" << std::endl;
	std::cout << "Total " << total << std::endl;
}

/**
* Displays the products stored in the shopping cart, and calculates the total amount.
* 
* @param conn Connection allows the connetion to the database
* @param cart ShoppingCart is the user shopping cart with the products stored in it.
* @param customerId int the number of the customer do access the customer's shopping cart
* @param productCount int is the number of products stored in the customer's shopping cart
*
*
*/
int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int
	productCount) {
	// yes/no option to checkout
	char option[2];
	int orderId = 0;
	int checkOut = 0;

	//While loop to check if the option to checkout is correct
	do {
		std::cout << "Would you like to checkout? (Y/y or N/n) ";
		std::cin >> option;

		if ((option[0] != 'y' && option[0] != 'Y' && option[0] != 'n' && option[0] != 'N') || (option[1] != '\0')) {
			std::cout << "Wrong input. Try again..." << std::endl;
		}
	} while ((option[0] != 'y' && option[0] != 'Y' && option[0] != 'n' && option[0] != 'N') || (option[1] != '\0'));

	// input as Y/y to checkout
	if (option[0] == 'y' || option[0] == 'Y') {
		
		//ADD ORDER to the database
		Statement* addOrder = conn->createStatement();
		addOrder->setSQL("begin add_order(:1, :2); end;");
		addOrder->setNumber(1, customerId);
		addOrder->registerOutParam(2, OCCIINT);
		addOrder->execute();

		orderId = addOrder->getInt(2);
		conn->terminateStatement(addOrder);

		//ADD ITEMS to the database
		Statement* addItem = conn->createStatement();
		addItem->setSQL("begin add_order_item(:1, :2, :3, :4, :5); end;");
		addItem->setNumber(1, orderId);
		for (int i = 0; i < productCount; i++) {
			addItem->setNumber(2, (i+1));
			addItem->setNumber(3, cart[i].product_id);
			addItem->setNumber(4, cart[i].quantity);
			addItem->setNumber(5, cart[i].price);
			addItem->execute();
		}

		conn->terminateStatement(addItem);
		
		checkOut = 1;
		std::cout << "The order was successfully completed." << std::endl;
	}
	return checkOut;
}

int main(void) {
	std::cout << "----------------ASSIGNMENT 2--------------------------" << std::endl;
	std::cout << "* COURSE: DBS311" << std::endl;
	std::cout << "* SECTION : NCC" << std::endl;
	std::cout << "* GROUP 11" << std::endl;
	std::cout << "* NAMES : " << std::endl;
	std::cout << "*" << "   Luigi Zaccagnini" << std::endl;
	std::cout << "*" << "   Pedro do Carmo" << std::endl;
	std::cout << "*" << "   Shin Taek Lee" << '\n' << '\n' << std::endl;

	Environment* env = nullptr;
	Connection* conn = nullptr;
	
	//informations to connect to the database
	std::string user = "dbs311_202c11";
	std::string pass = "27884552";
	std::string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";

	try {
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, pass, constr);
		//Pre defined option in order to be cought in the while loop
		int option = 2;
		//Customer cart
		ShoppingCart* cart;
		int itemsCounter;
		int custId;
		// initial default of foundCust. No customer found
		int foundCust = 0;
		
		//While different from 0 the user stays in the system
		while(option != 0){
			std::cout << "******************** Main Menu ********************" << std::endl;
			std::cout << "1) Login" << std::endl;
			std::cout << "0) Exit" << std::endl;
			std::cout << "Enter an option(0 - 1): ";
			std::cin >> option;

			//Option must be 0 or 1 to move foward
			while (option != 0 && option != 1){
				std::cout << "******************** Main Menu ********************" << std::endl;
				std::cout << "1) Login" << std::endl;
				std::cout << "0) Exit" << std::endl;
				std::cout << "You entered a wrong value. Enter an option(0 - 1): ";

				std::cin >> option;

				}

			//Option 1 the user can login.
			if (option == 1) {
				std::cout << "Enter the customer ID: ";
				std::cin >> custId;
				foundCust = customerLogin(conn, custId);

				//if the login was not found in the database
				if (foundCust == 0) {
					std::cout << "The Customer does not exist." << std::endl;
				}

				//if the login was found in the database.
				if (foundCust == 1) {
					std::cout << "-------------- Add Products to Cart --------------" << std::endl;
					cart = new ShoppingCart[5];
					itemsCounter = addToCart(conn, cart);

					std::cout << "------- Ordered Products---------" << std::endl;
					displayProducts(cart, itemsCounter);
					checkout(conn, cart, custId, itemsCounter);
				}
			} 
		}

		std::cout << "Good bye!...";

		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
		
	}
	//catch any connections error
	catch (SQLException & sqlExcp) {
		std::cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
	
	return 0;
}
