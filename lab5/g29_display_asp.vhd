library ieee;
use ieee.std_logic_1164.all;
use work.types.all;

entity g29_display_asp is
	port (
		clock : in std_logic;
		display : in std_logic;
		player_ready: out std_logic;
		ASP_longest : in natural range 0 to 7;
		ASP_length : in natural range 0 to 4;
		KEY_ready : in std_logic;
		
		-- ASP
		asp_idx: out natural range 0 to 7;  -- index of desired ASP register
		asp_row_read, asp_col_read: in ASP_register;  -- ASP row and col read from ASP Registers Module
		write_enable_asp: out std_logic  -- active low		
	);
end entity g29_display_asp;

architecture RTL of g29_display_asp is
	
begin
	process(clock)
	begin
		if rising_edge(clock) then
			if KEY_ready = '0' then
				player_ready <= '0';
			else
				player_ready <= '1';
			end if;
		end if;
	end;

	asp_idx <= ASP_longest;
	write_enable_asp <= '1';

end architecture RTL;
