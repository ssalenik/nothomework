-- package containing various types needed
--
--
-- Version 1.0
-- Author: Stepan Salenikovich, Ayman Zakir
-- Date: 30.09.2011

library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;
use ieee.numeric_std.all; -- functions for converting between std_logic and int

use work.types.all;

package type_conversion is

		-- conversts an array of 12 bits to an array of 3-bit integers of length 4
		function array_12bit_to_array_4int (bit_12_array : std_logic_vector(11 downto 0)) return ASP_register;
		
		-- conversts an array of 3-bit integers of length 4 to an array of 12 bits
		function array_4int_to_array_12bit (int_4_array : ASP_register) return std_logic_vector;
		
end type_conversion;

package body type_conversion is

	function array_12bit_to_array_4int (bit_12_array : std_logic_vector(11 downto 0)) return ASP_register is
		variable bit_3_array : std_logic_vector(2 downto 0); -- temp stores a 3 bits of the 12
		variable result : ASP_register;  -- the resulting 4 int array
		
	begin
		
		for array_idx in 0 to 3 loop --goes through the 4 3bit numbers in the 12bit array
			
			for num_idx in 0 to 2 loop --goes through each 3bit number
				
				bit_3_array(num_idx) := bit_12_array( (array_idx)*3 + num_idx);
				
			end loop;
			
			--convert the 3bit number to int and insert into int array
			result(array_idx) := to_integer( unsigned(bit_3_array) );
			
		end loop;
		
		return result;
		
	end function;
	
	
	
	function array_4int_to_array_12bit (int_4_array : ASP_register) return std_logic_vector is
		variable bit_3_array : std_logic_vector(2 downto 0); -- temp stores a 3 bits of the 12
		variable result : std_logic_vector(11 downto 0);  -- the resulting 12 bit array
		
	begin
		
		for array_idx in 0 to 3 loop --goes through the 4 int array
					
			bit_3_array := std_logic_vector(to_unsigned(int_4_array(array_idx), bit_3_array'length));
			
			for num_idx in 0 to 2 loop --goes through each 3bit number
				result( (array_idx)*3 + num_idx ) := bit_3_array(num_idx);
			end loop;
			
		end loop;
		
		return result;
		
	end function;
	
end package body;