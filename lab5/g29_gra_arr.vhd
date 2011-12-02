-- GRA Array Module
--
-- entity name: g29_gra_arr
--
-- Version 1.0
-- Author: Stepan Salenikovich, Ayman Zakir
-- Date: 24.11.2011

library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;
use work.types.all;

entity g29_gra_arr is
	port(
		row_in, col_in: in natural range 0 to 7; -- row and col index
		gra_in: in std_logic_vector(0 to 1); -- write cell state: 00 - empty, 01 - yellow, 11 - blue
		write_enable: in std_logic;  -- active low
		clk: in std_logic;
		rst: in std_logic; -- resets GRA to all empty cells, active low
		gra_out: out std_logic_vector(0 to 1) -- read cell state: 00 - empty, 01 - yellow, 11 - blue, 10 - out of bounds
	);
end g29_gra_arr;

architecture logic of g29_gra_arr is
	signal GRA : GRA_array := (others=> (others => "00")); -- GRA grid, encoded as 2 bits for each cell x 7 columns x 6 rows = 84 bits
begin
	process(clk, rst)
		variable gra_idx0, gra_idx1 : natural range 0 to 83;
	begin	
		if rst = '0' then
			GRA <= (others => (others => "00"));			
		elsif rising_edge(clk) then
			--bounds checking
			if row_in < 6 and col_in < 7 and write_enable = '0' then
				GRA(row_in)(col_in) <= gra_in;
			end if;			
		end if;		
	end process;
	-- output "10" if out of bounds
	gra_out <= GRA(row_in)(col_in) when row_in < 6 and col_in < 7 else "10";
end logic;