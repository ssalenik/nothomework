library ieee;
use ieee.std_logic_1164.all;

entity g29_controller is
	port (
		clock : in std_logic;
		KEY_reset : in std_logic;
		-- controll signals
		game_init, display: out std_logic;
		display_end: out natural range 0 to 2;
		game_init_complete, player_ready : in std_logic;
		NM : out std_logic;
		inASPMC, inSPMC, disk_ins : in std_logic;
		ASPMC, SPMC : out std_logic;
		end_config : in std_logic;
		start_config, start_incr: out std_logic;
		-- 0: game init, 1: display_asp, 2: sp_player, 3: asp_player, 4: end game
		asp_access, gra_access, display_access : out natural range 0 to 4;
		-- info signals
		mv_count : in natural range 0 to 16;
		inASP_longest : in natural range 0 to 7;
		inASP_length : in natural range 0 to 4;
		ASP_idx : out natural range 0 to 7;
		ASP_entry : out natural range 0 to 4;
		blah : in std_logic
	);
end entity g29_controller;

architecture RTL of g29_controller is
	type state_type is (start, sp_move, asp_move, display_asp, end_game);
	signal state : state_type := start;
	signal ASP_longest : natural range 0 to 7 := 0;
	signal ASP_length : natural range 0 to 4 := 0;
begin
	process(clock, KEY_reset)
	
	begin
		if KEY_reset then
			state <= start;
			game_init <= '0';
			display <= '1';
			display_end <= 0;
			NM <= '1';
			ASPMC <= '1';
			SPMC <= '1';
			start_config <= '1';
			start_incr <= '1';
			display_access <= 0;
			asp_access <= 0;
			gra_access <= 0;
		elsif rising_edge(clock) then
			if state = start then
				if game_init_complete = '0' then
					state <= display_asp;
					ASP_longest <= 0;
					ASP_length <= 1;
					display <= '0';	
					game_init <= '1';
					asp_access <= 1;
					display_access <= 1;
				end if;
			elsif state = display_asp then
				if player_ready = '0' then
					state <= sp_move;
					display <= '1';
					NM <= '0';
					ASPMC <= '0';
					display_access <= 2;
					gra_access <= 2;
				end if;	
			elsif state = sp_move then
				if inSPMC = '0' then
					if mv_count < 16 then
						state <= asp_move;
						SPMC <= '0';
						start_config <= '0';
						start_incr <= '1';
						display_access <= 1;
						gra_access <= 3;
						asp_access <= 3;
					else
						state <= end_game;
						display_access  <= 4;
					end if;
				end if;
			elsif state = asp_move then
				if end_config = '0' and inASPMC = '1' then
					start_config <= '1';
					start_incr <= '0';
				elsif inASPMC = '0' then
					if disk_ins = '0' then
						if inASP_length = 4 then
							state <= end_game;
							display_access  <= 4;
							NM <= '1';
						else
							state <= display_asp;
							display_access  <= 1;
							asp_access <= 1;
							gra_access <= 1;
							ASPMC <= '0';
						end if;
						ASP_longest <= inASP_longest;
						ASP_length <= inASP_length;
					else
						state <= end_game;
						display_access  <= 4;
						NM <= '1';
					end if;
					display <= '0';
				end if;
			elsif state = end_game then
				if ASP_length = 4 then
					display_end <= 2;
					-- dislay "LOSE"
				else
					display_end <= 1;
					-- display "END"
				end if;
			end if;
		end if;
	end;
	
	ASP_idx <= ASP_longest;
	ASP_entry <= ASP_length;
end architecture RTL;
