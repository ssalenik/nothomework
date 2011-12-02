-- Determining the longest row stored in the ASP registers
--
-- entity name: g29_longest_row
--
-- Version 1.0
-- Author: Stepan Salenikovich, Ayman Zakir
-- Date: 05.10.2011

library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;
use ieee.numeric_std.all; -- functions for converting between std_logic and int

use work.types.all; --package with types
use work.type_conversion.all; --package with type conversion

entity g29_longest_row is
	port(ASP_r_0, ASP_r_1, ASP_r_2, ASP_r_3, ASP_r_4, ASP_r_5, ASP_r_6, ASP_r_7: in std_logic_vector(11 downto 0);
	ASP_c_0, ASP_c_1, ASP_c_2, ASP_c_3, ASP_c_4, ASP_c_5, ASP_c_6, ASP_c_7: in std_logic_vector(11 downto 0);
	ASP_dr: in std_logic_vector(0 to 7);
	ASP_empty: in std_logic_vector(0 to 7);
	NM: in std_logic;
	
	ASP_longest: out std_logic_vector(2 downto 0);
	
	-- for testing:
	ASP_r_out: out std_logic_vector(11 downto 0);
	ASP_length_out: out std_logic_vector(2 downto 0));
end g29_longest_row;

architecture logic of g29_longest_row is

	
begin

	-- calculates the longest row
	-- note: longest row output will be '000' if all rows have length 0
	process(NM)
	
		variable ASP_r, ASP_c : ASP_array; -- used to store the ASP registers in integer format
		variable ASP_idx : natural range 0 to 7 := 0; -- index of ASP register
		variable reg_idx : natural range 0 to 3 := 0; -- index of int in ASP register
		variable temp_ASP_length, ASP_length : natural range 0 to 4 := 0; -- length of longest row
		variable temp_ASP_longest : natural range 0 to 7 := 0; -- idx of longest row
							
	begin
	
		-- NM essentially as a clock here, nothing is done until the rising edge of NM
		if NM = '1' and NM'event then
		
			-- converts the ASP inputs to a 2D array of 7 registers, each with 4 3bit ints, for both the rows and the columns

			-- go through the row ASP registers
			ASP_r (0) := array_12bit_to_array_4int(ASP_r_0);
			ASP_r (1) := array_12bit_to_array_4int(ASP_r_1);
			ASP_r (2) := array_12bit_to_array_4int(ASP_r_2);
			ASP_r (3) := array_12bit_to_array_4int(ASP_r_3);
			ASP_r (4) := array_12bit_to_array_4int(ASP_r_4);
			ASP_r (5) := array_12bit_to_array_4int(ASP_r_5);
			ASP_r (6) := array_12bit_to_array_4int(ASP_r_6);
			ASP_r (7) := array_12bit_to_array_4int(ASP_r_7);

			-- go through the column ASP registers
			ASP_c (0) := array_12bit_to_array_4int(ASP_c_0);
			ASP_c (1) := array_12bit_to_array_4int(ASP_c_1);
			ASP_c (2) := array_12bit_to_array_4int(ASP_c_2);
			ASP_c (3) := array_12bit_to_array_4int(ASP_c_3);
			ASP_c (4) := array_12bit_to_array_4int(ASP_c_4);
			ASP_c (5) := array_12bit_to_array_4int(ASP_c_5);
			ASP_c (6) := array_12bit_to_array_4int(ASP_c_6);
			ASP_c (7) := array_12bit_to_array_4int(ASP_c_7);
			
			-- cycle through the 8 ASP registers
			for ASP_idx in 0 to 7 loop
				
				-- continue if current register is not a "dead row" and not empty
				if ( ASP_dr(ASP_idx) = '0' ) and ( ASP_empty(ASP_idx) = '0' )  then
					
					temp_ASP_length := 0;  -- reset value of length
					
					-- cycle through the 4 ints in each row and column ASP register
					for reg_idx in 0 to 3 loop
						
						-- check that value != 7 and increment length of row
						if ( ASP_r(ASP_idx)(reg_idx) /= 7 ) and ( ASP_c(ASP_idx)(reg_idx) /=7 ) then
							temp_ASP_length := temp_ASP_length + 1;
						end if;
						
					end loop;
					
					-- if current ASP is longer than previous, set it as the new longest
					if temp_ASP_length > ASP_length then
						ASP_length := temp_ASP_length;
						temp_ASP_longest := ASP_idx;
					end if;	
					
				end if;
			
			end loop;
			
			ASP_longest <= std_logic_vector(to_unsigned(temp_ASP_longest, ASP_longest'length));
			
			-- for testing
			ASP_r_out <= array_4int_to_array_12bit( ASP_r(2));
			ASP_length_out <= std_logic_vector(to_unsigned(ASP_length, ASP_length_out'length));
			
		end if;	
		
	end process;

end logic;