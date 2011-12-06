-- determines next ASP move if longest row is not dead or empty
-- 
-- entity name: g29_ASP_next_move
--
-- Version 1.0
-- Author: Stepan Salenikovich, Ayman Zakir
-- Date: 18.11.2011

library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;
use ieee.numeric_std.all; -- functions for converting between std_logic and int
use work.types.all;

entity g29_ASP_next_move is
	port(
		-- INPUTS --
		NM: in std_logic; --Next Move, active low
		start_config : in std_logic;
		start_incr: in std_logic;  --start to detrming configuration
		end_config: out std_logic;
		clk, rst: in std_logic; -- clock and reset

		inASP_dr: in std_logic_vector(0 to 7); -- dead row register
		inASP_empty: in std_logic_vector(0 to 7); -- empty row register
		inASP_length: in natural range 0 to 4; -- length of longest row
		inASP_longest: in natural range 0 to 7; -- index of the longest ASP register
		
		-- for initializing GRA for testing
		gra_write_init: in std_logic_vector(0 to 1);
		gra_monitor: out std_logic_vector(0 to 1);
		gra_row_init, gra_col_init: in natural range 0 to 7;
		write_enable_gra_init : in std_logic; -- active low
		
		-- for initializing ASP for testing
		write_enable_asp_init: in std_logic;
		asp_idx_init: in natural range 0 to 7;
		asp_row_write_init, asp_col_write_init: in ASP_register;

		-- for monitoring ASP for testing
		asp_idx_monitor: out natural range 0 to 7;
		asp_row_read_monitor, asp_col_read_monitor: out ASP_register;
		
		-- for monitoring configuraiton for testing
		horiz_monitor, vertic_monitor, lr_diag_monitor, rl_diag_monitor : out std_logic;
		
		-- OUTPUTS --
		disk_ins: out std_logic;
		ASPMC: out std_logic;
		ASP_dr: out std_logic_vector(0 to 7);
		ASP_empty: out std_logic_vector(0 to 7);
		ASP_longest : out natural range 0 to 7;
		ASP_length : out natural range 0 to 4
);
end g29_ASP_next_move;

architecture concurrent of g29_ASP_next_move is

	signal horiz, vertic, lr_diag, rl_diag : std_logic;
	-- GRA
	signal gra_read, gra_write, gra_write_selected : STD_LOGIC_VECTOR(0 TO 1);
	signal gra_row_selected, gra_row, gra_row_conf, gra_row_incr : natural range 0 to 7;
	signal gra_col_selected, gra_col, gra_col_conf, gra_col_incr: natural range 0 to 7;
	signal write_enable_gra_selected, write_enable_gra, write_enable_gra_conf, write_enable_gra_incr: std_logic;
	--ASP
	signal write_enable_asp, write_enable_asp_conf, write_enable_asp_incr, write_enable_asp_selected : std_logic;
	signal asp_idx_incr, asp_idx, asp_idx_selected: natural range 0 to 7;
	signal asp_row_read, asp_col_read, asp_row_write, asp_row_write_selected, asp_col_write, asp_col_write_selected : ASP_register;
begin
				
	with NM select gra_write_selected <=
				gra_write when '0',
				gra_write_init when others;
				
	with start_incr select gra_row <=
				gra_row_conf when '1',
				gra_row_incr when others;
	
	with NM select gra_row_selected <=
				gra_row when '0',
				gra_row_init when others;
	
	with start_incr select gra_col <=
				gra_col_conf when '1',
				gra_col_incr when others;
				
	with NM select gra_col_selected <=
				gra_col when '0',
				gra_col_init when others;
	
	with start_incr select write_enable_gra <=
				write_enable_gra_conf when '1',
				write_enable_gra_incr when others;
				
	with NM select write_enable_gra_selected <=
				write_enable_gra when '0',
				write_enable_gra_init when others;
				
	with NM select asp_row_write_selected <=
				asp_row_write when '0',
				asp_row_write_init when others;
	
	with NM select asp_col_write_selected <=
				asp_col_write when '0',
				asp_col_write_init when others;
				
	with start_incr select asp_idx <=
				inASP_longest when '1',
				asp_idx_incr when others;
				
	with NM select asp_idx_selected <=
				asp_idx when '0',
				asp_idx_init when others;
	
	with start_incr select write_enable_asp <=
				write_enable_asp_conf when '1',
				write_enable_asp_incr when others;			
	
	with NM select write_enable_asp_selected <=
				write_enable_asp when '0',
				write_enable_asp_init when others;
	
	config : entity work.g29_row_config
		port map(clk              => clk,
			     NM               => NM,
			     start_config     => start_config,
			     inASP_length     => inASP_length,
			     asp_row_read     => asp_row_read,
			     asp_col_read     => asp_col_read,
			     write_enable_asp => write_enable_asp_conf,
			     gra_read         => gra_read,
			     gra_row          => gra_row_conf,
			     gra_col          => gra_col_conf,
			     write_enable_gra => write_enable_gra_conf,
			     horiz            => horiz,
			     vertic           => vertic,
			     lr_diag          => lr_diag,
			     rl_diag          => rl_diag,
			     end_config       => end_config);
			     
	AI : entity work.g29_row_incr
		port map(NM               => NM,
			     start_incr       => start_incr,
			     clk              => clk,
			     rst              => rst,
			     horiz         	  => horiz,
			     vertic           => vertic,
			     lr_diag          => lr_diag,
			     rl_diag          => rl_diag,
			     inASP_dr         => inASP_dr,
			     inASP_empty      => inASP_empty,
			     inASP_length     => inASP_length,
			     inASP_longest    => inASP_longest,
			     gra_read         => gra_read,
			     gra_write        => gra_write,
			     gra_row          => gra_row_incr,
			     gra_col          => gra_col_incr,
			     write_enable_gra => write_enable_gra_incr,
			     asp_idx          => asp_idx_incr,
			     asp_row_read     => asp_row_read,
			     asp_col_read     => asp_col_read,
			     asp_row_write    => asp_row_write,
			     asp_col_write    => asp_col_write,
			     write_enable_asp => write_enable_asp_incr,
			     disk_ins         => disk_ins,
			     ASPMC            => ASPMC,
			     ASP_dr           => ASP_dr,
			     ASP_empty        => ASP_empty,
			     ASP_longest	  => ASP_longest,
			     ASP_length	  	  => ASP_length);
									
	GRA : entity work.g29_gra_arr
		port map(row_in       => gra_row_selected,
			     col_in       => gra_col_selected,
			     gra_in       => gra_write_selected,
			     write_enable => write_enable_gra_selected,
			     clk          => clk,
			     rst          => rst,
			     gra_out      => gra_read);
			     
	ASP : entity work.g29_asp_registers
		port map(asp_idx      => asp_idx_selected,
			     asp_row_in   => asp_row_write_selected,
			     asp_col_in   => asp_col_write_selected,
			     write_enable => write_enable_asp_selected,
			     clk          => clk,
			     rst          => rst,
			     asp_row_out  => asp_row_read,
			     asp_col_out  => asp_col_read);	

	gra_monitor <= gra_read;
	asp_idx_monitor <= asp_idx_selected;
	asp_row_read_monitor <= asp_row_read;
	asp_col_read_monitor <= asp_col_read;
	horiz_monitor <= horiz;
	vertic_monitor <= vertic;
	lr_diag_monitor <= lr_diag;
	rl_diag_monitor <= rl_diag;
	
end concurrent;