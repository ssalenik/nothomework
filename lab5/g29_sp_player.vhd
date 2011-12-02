-- SP player 
--
-- entity name: g29_sp_player
--
-- Version 1.1
-- Author: Stepan Salenikovich, Ayman Zakir
-- Date: 25.11.2011

library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;

entity g29_sp_player is
	port(
		NM, ASPMC: in std_logic;
		-- I/O ports for communication with the GRA array
		gra_read: in std_logic_vector(0 to 1);  
		write_enable: out std_logic;  -- active low
		gra_row, gra_col : out natural range 0 to 7; -- row & col index
		gra_write: out std_logic_vector(0 to 1);
		-- cyclone II board I/O
		SW_row, SW_col: in natural range 0 to 7;
		SW_gra_value: in std_logic_vector(0 to 1);
		KEY_write: in std_logic; -- push to write
		--
		clk, rst: in std_logic;
		--
		mc_count: out natural range 0 to 16;
		SPMC: out std_logic);
end g29_sp_player;

architecture logic of g29_sp_player is
	signal count : natural range 0 to 16 := 0;
	signal row, col : natural range 0 to 7; -- save previous row & col index
begin
	process(clk, rst)
	begin
		if rst = '0' then
			count <= 0;
		elsif rising_edge(clk) then
			if NM = '0' and ASPMC = '0' then
				gra_write <= SW_gra_value;
				-- save row and column
				row <= SW_row;
				col <= SW_col;
				
				-- when key is pressed, the gra_read is the value of the row and column selected on the previous clock cycle
				-- so use the values saved on the last clock cycle to select row and column
				-- do not let user write to GRA cell unless its empty, SP can only set it to blue
				if KEY_write = '0' and gra_read = "00" and SW_gra_value = "11" then
					-- write to save row and column
					write_enable <= '0';
					gra_row <= row;
					gra_col <= col;
					-- increment count
					count <= count + 1;
					SPMC <= '1';
				else
					write_enable <= '1';
					gra_row <= SW_row;
					gra_col <= SW_col;
					SPMC <= '0';
				end if;
			end if;
		end if;
	end process;
	mc_count <= count;
end logic;