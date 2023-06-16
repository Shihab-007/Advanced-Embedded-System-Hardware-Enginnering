library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all; -- Update the library import

entity VendingMachine is
  port (
    clk : in std_logic;
    reset : in std_logic;
    coin : in std_logic_vector(1 downto 0);
    product1 : in std_logic;
    product2 : in std_logic;
    product3 : in std_logic;
    dispense : out std_logic;
    refund : out std_logic
  );
end entity VendingMachine;

architecture Behavioral of VendingMachine is
  type state_type is (idle, product_select, insert_coin, check, dispensing, refunding);
  signal current_state, next_state : state_type;
  signal change_state : std_logic;
  signal amount : unsigned(7 downto 0);
  constant product1_price : unsigned(7 downto 0) := to_unsigned(20, 8);  -- Price for product 1 (20 cents)
  constant product2_price : unsigned(7 downto 0) := to_unsigned(50, 8);  -- Price for product 2 (50 cents)
  constant product3_price : unsigned(7 downto 0) := to_unsigned(100, 8); -- Price for product 3 (1 euro)

begin
  -- Sequential process to change states
  state_process : process(clk, reset)
  begin
    if reset = '1' then
      current_state <= idle;
      amount <= (others => '0');
      change_state <= '0';
    elsif rising_edge(clk) then
      if change_state = '1' then
        current_state <= next_state;
      end if;
    end if;
  end process state_process;

  -- Combinational process to determine next states and outputs
  vending_process : process(current_state, coin, product1, product2, product3, amount)
  begin
    next_state <= current_state;

    case current_state is
      when idle =>
        if product1 = '1' then
          next_state <= product_select;
        elsif product2 = '1' then
          next_state <= product_select;
        elsif product3 = '1' then
          next_state <= product_select;
        end if;
        
      when product_select =>
        if coin = "01" or coin = "10" or coin = "11" then -- Valid coin denominations
          next_state <= insert_coin;
          case amount is -- Use 'amount' instead of 'product1_price' here
            when product1_price => amount <= product1_price;
            when product2_price => amount <= product2_price;
            when product3_price => amount <= product3_price;
            when others => null; -- Unexpected state
          end case;
        end if;
        
      when insert_coin =>
        if coin = "01" or coin = "10" or coin = "11" then -- Valid coin denominations
          amount <= amount + unsigned(coin);
        end if;
        
        if (amount >= product1_price and product1 = '1') or
           (amount >= product2_price and product2 = '1') or
           (amount >= product3_price and product3 = '1') then
          next_state <= check;
        end if;
        
      when check =>
        if amount >= product1_price and product1 = '1' then
          next_state <= dispensing;
          amount <= amount - product1_price;
        elsif amount >= product2_price and product2 = '1' then
          next_state <= dispensing;
          amount <= amount - product2_price;
        elsif amount >= product3_price and product3 = '1' then
          next_state <= dispensing;
          amount <= amount - product3_price;
        else
          next_state <= refunding;
        end if;
        
      when dispensing =>
        next_state <= idle;
        
      when refunding =>
        next_state <= idle;
        
    end case;

    change_state <= '1'; -- Set the signal to indicate state change
  end process vending_process;

  -- Output assignments
  dispense <= '1' when current_state = dispensing else '0';
  refund <= '1' when current_state = refunding else '0';

end architecture Behavioral;
