-- display decoder
--
-- entity name: g29_7_segment_decoder
--
-- Version 1.2
-- Author: Ayman Zakir, Stepan Salenikovich
-- Date: 25.11.2011

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library lpm;
use lpm.lpm_components.all;

entity g29_7_segment_decoder is
	port(
		clk: in std_logic;
		number: in natural range 0 to 7;
		display_end : in natural range 0 to 2;
		seven_seg_display: out std_logic_vector(6 downto 0)
	);
end g29_7_segment_decoder;

architecture display of g29_7_segment_decoder is
	signal lut_output : std_logic_vector(6 downto 0);
	constant LOSE : std_logic_vector(6 downto 0) := "11110000";
	constant ENDGAME : std_logic_vector(6 downto 0) := "00001111";
begin
	crc_table : lpm_rom                       -- use the altera rom library macrocell
	
	GENERIC MAP(
		lpm_widthad => 3,                     -- sets the width of the ROM address bus
		lpm_numwords => 8,                  -- sets the words stored in the ROM
		lpm_outdata => "UNREGISTERED",        -- no register on the output
		lpm_address_control => "REGISTERED",  -- register on the input
		lpm_file => "g29_3_bit_table.mif",   -- the ascii file containing the rom data
		lpm_width => 7                        -- the width of the word stored in each ROM location
	) 
	
	PORT MAP(inclock => clk, address => std_logic_vector(to_unsigned(number, 3)), q => lut_output);
	
	with display_end select seven_seg_display <=
					lut_output when display_end = 0,
					ENDGAME when display_end = 1,
					LOSE when display_end = 2;
	
end display;