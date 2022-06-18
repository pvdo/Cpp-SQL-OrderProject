
-- FIND CUSTOMER PROCEDURE
CREATE OR REPLACE PROCEDURE find_customer(cust_id IN NUMBER, found OUT NUMBER) AS
cust NUMBER(9,2);
BEGIN
    SELECT customer_id INTO cust
    FROM customers
    WHERE customer_id = cust_id;
    IF sql%rowcount > 0 
    THEN
     found := 1;
    END IF;
EXCEPTION
    WHEN no_data_found THEN found := 0;
END;
/

SET SERVEROUT ON

DECLARE
abc NUMBER;
BEGIN
    find_customer(2,abc);
    DBMS_OUTPUT.PUT_LINE(abc);
END;
-- FIND PRODUCT

CREATE OR REPLACE PROCEDURE find_product (prod_id IN NUMBER, price OUT products.list_price%TYPE) AS
prod NUMBER;
proPrice products.list_price%TYPE;
BEGIN
    SELECT product_id, list_price INTO prod, proPrice
    FROM products
    WHERE product_id = prod_id;
    IF sql%rowcount > 0 THEN
     price := proPrice;
    END IF;
EXCEPTION
    WHEN no_data_found THEN
    price := 0;
END; 
DECLARE
abc NUMBER(9,2);
BEGIN
    find_product(1000,abc);
    DBMS_OUTPUT.PUT_LINE(abc);
END;
-- ADD ORDER
CREATE OR REPLACE PROCEDURE add_order (cust_id IN NUMBER, new_order_id OUT NUMBER) AS
BEGIN
    SELECT MAX(order_id) + 1 INTO new_order_id
    FROM orders;
    
    INSERT INTO orders(order_id, customer_id, status, salesman_id, order_date) values (new_order_id, cust_id, 'Shipped', 56, sysdate);
    
END;

-- ADD ORDER ITEM

CREATE OR REPLACE PROCEDURE add_order_item(orderId IN order_items.order_id%type,
                                            itemId IN order_items.item_id%type,
                                            productId IN order_items.product_id%type,
                                            qty IN order_items.quantity%type,
                                            price IN order_items.unit_price%type) AS
BEGIN
    INSERT INTO order_items(order_id, item_id, product_id, quantity, unit_price) values (orderId, itemId, productId, qty, price);
END;
