-- ASP Registers Module
--
-- entity name: g29_asp_registers
--
-- Version 1.0
-- Author: Stepan Salenikovich, Ayman Zakir
-- Date: 24.11.2011

library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;
use work.types.all; --package with types and relevant functions

entity g29_asp_registers is
	port(
		asp_idx: in natural range 0 to 7;  -- index of desired ASP register
		asp_row_in, asp_col_in: in ASP_register;  -- ASP row and col to be wirtten
		write_enable: in std_logic;  -- active low
		--
		clk: in std_logic;
		rst: in std_logic; -- resets ASP to all empty; active low
		--
		asp_row_out, asp_col_out: out ASP_register);  -- output ASP row and col
end g29_asp_registers;

architecture logic of g29_asp_registers is
	signal ASP_r, ASP_c : ASP_array := ((others=> (others=>7)));
begin
	process(clk, rst)
	begin
		if rst = '0' then
			ASP_r <= ((others => (others => 7)));
			ASP_c <= ((others => (others => 7)));
		elsif rising_edge(clk) then
			if write_enable = '0' then
				-- write to ASP
				ASP_r(asp_idx)<= asp_row_in;
				ASP_c(asp_idx)<= asp_col_in;
			end if;
		end if;	
	end process;
	asp_row_out <= ASP_r(asp_idx);
	asp_col_out <= ASP_c(asp_idx);		
end logic;