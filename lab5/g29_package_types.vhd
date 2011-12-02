-- package containing various types needed
-- and functions pertaining to those types
--
--
-- Version 2.0
-- -- 1.0 - first version - containing in arrays for ASP register and 2D array of ASP registers
-- -- 1.1 - merged with typ_conversion package
-- -- 2.0 - with lab 4
--
-- Author: Stepan Salenikovich, Ayman Zakir
-- Date: 16.11.2011

library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;
use ieee.numeric_std.all; -- functions for converting between std_logic and int

package types is
	type ASP_register is array (0 to 3) of natural range 0 to 7;
	type ASP_array is array (0 to 7) of ASP_register;
	
	type GRA_column is array (0 to 6) of std_logic_vector(0 to 1);
	type GRA_array is array (0 to 5) of GRA_column;
	--type ASP_array is array (0 to 7) of std_logic_vector(11 downto 0);	
end types;