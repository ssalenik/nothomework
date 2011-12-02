library ieee;
use ieee.std_logic_1164.all;
use work.types.all;

entity g29_game_init is
	port (
		clock : in std_logic;
		reset : in std_logic;
		game_init: in std_logic; --active low
		-- GRA
		write_enable_gra: out std_logic;  -- active low
		gra_row, gra_col : out natural range 0 to 7; -- row & col index
		gra_write: out std_logic_vector(0 to 1);
		-- ASP
		asp_idx: out natural range 0 to 7;  -- index of desired ASP register
		asp_row_write, asp_col_write: out ASP_register;  -- ASP row and col to be wirtten
		write_enable_asp: out std_logic;  -- active low
		-- cyclone II board I/O
		SW_row, SW_col: in natural range 0 to 7;
		KEY_write: in std_logic; -- push to write
		--
		game_init_complete: out std_logic -- active low
	);
end entity g29_game_init;

architecture RTL of g29_game_init is
	signal write_action : std_logic := '1';
begin
	process(clock)
	begin
		if reset = '0' then
			write_enable_gra <= '1';
			write_enable_asp <= '1';
			game_init_complete <= '1';
		elsif rising_edge(clock) and game_init = '0' then
			if KEY_write = '0' then
				write_enable_gra <= '0';
				write_enable_asp <= '0';
				game_init_complete <= '0';
			else
				write_enable_gra <= '1';
				write_enable_asp <= '1';
				game_init_complete <= '1';
			end if;
		end if;
	end;
			
	gra_write <= "01";
	asp_idx <= 0;
	
	gra_row <= SW_row;
	gra_col <= SW_col;
	
	asp_row_write <= (0 => SW_row, others => 7);
	asp_col_write <= (0 => SW_col, others => 7);
	
end architecture RTL;
