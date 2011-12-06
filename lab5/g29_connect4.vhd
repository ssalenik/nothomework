library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;
use work.types.all;
--library work;

entity g29_connect4 is
	port (	clock : in std_logic;

			KEY_reset : in std_logic;
			KEY_ready : in std_logic;
			KEY_write : in std_logic;
			
			SW_row, SW_col: in natural range 0 to 7;
			SW_gra_value: in std_logic_vector(0 to 1);
			
			LEDR_row, LEDR_col: out natural range 0 to 7; -- LEDs light up when value high
			LEDR_gra_value: out std_logic_vector(0 to 1);
			
			HEX0: out std_logic_vector(6 downto 0);
			HEX1: out std_logic_vector(6 downto 0);
			HEX2: out std_logic_vector(6 downto 0);
			HEX3: out std_logic_vector(6 downto 0)
	);
end g29_connect4;

architecture everything of g29_connect4 is
	-- controller signals
	signal display_end, game_init, display, game_init_complete, player_ready,
	NM, inASPMC, inSPMC, disk_ins, ASPMC, SPMC,
	end_config, start_config, start_incr : std_logic;
	signal asp_access, gra_access, display_access: natural range 0 to 4;
	signal mv_count : natural range 0 to 16;
	signal inASP_longest, outASP_longest : natural range 0 to 7;
	signal inASP_length, outASP_length : natural range 0 to 4;
	signal inASP_dr, inASP_empty, ASP_dr, ASP_empty : std_logic_vector(0 to 7);
	--
	signal horiz, vertic, lr_diag, rl_diag : std_logic;
	-- GRA
	signal gra_read: STD_LOGIC_VECTOR(0 TO 1);
	signal gra_write, gra_write_init, gra_write_sp, gra_write_asp, gra_write_incr, gra_write_conf : STD_LOGIC_VECTOR(0 TO 1);
	signal gra_row, gra_row_init, gra_row_disp, gra_row_sp, gra_row_asp, gra_row_conf, gra_row_incr : natural range 0 to 7;
	signal gra_col, gra_col_init, gra_col_disp, gra_col_sp, gra_col_asp, gra_col_conf, gra_col_incr : natural range 0 to 7;
	signal write_enable_gra, write_enable_gra_init, write_enable_gra_disp, write_enable_gra_sp, write_enable_gra_asp, write_enable_gra_conf, write_enable_gra_incr: std_logic;
	--ASP
	signal write_enable_asp, write_enable_asp_init, write_enable_asp_disp, write_enable_asp_asp, write_enable_asp_conf, write_enable_asp_incr : std_logic;
	signal asp_idx, asp_idx_init, asp_idx_disp, asp_idx_asp, asp_idx_incr, asp_idx_conf: natural range 0 to 7;
	signal asp_row_read, asp_col_read: ASP_register;
	signal asp_row_write, asp_row_write_init, asp_row_write_asp, asp_row_write_incr, asp_row_write_conf : ASP_register;
	signal asp_col_write, asp_col_write_asp, asp_col_write_init, asp_col_write_incr, asp_col_write_conf : ASP_register;
	--
	signal disp0, disp1, disp2, disp3: natural range 0 to 7;

begin

	-- multilexing using control signals
	-- 0: game init, 1: display_asp, 2: sp_player, 3: asp_player, 4: end game
	-- GRA:
	with gra_access select gra_write <=
				gra_write_init when 0,
				gra_write_sp when 2,
				gra_write_asp when 3,
				'X' when others;
	
	with gra_access select write_enable_gra <=
				write_enable_gra_init when 0,
				write_enable_gra_sp when 2,
				write_enable_gra_asp when 3,
				'X' when others;
				
	with gra_access select gra_row <=
				gra_row_init when 0,
				gra_row_disp when 1,
				gra_row_sp when 2,
				gra_row_asp when 3,
				'X' when others;
				
	with gra_access select gra_row <=
				gra_row_init when 0,
				gra_row_disp when 1,
				gra_row_sp when 2,
				gra_row_asp when 3,
				'X' when others;
				
	-- ASP registers:
	with asp_access select write_enable_asp <=
				write_enable_asp_init when 0,
				write_enable_asp_disp when 1,
				write_enable_asp_asp when 3,
				'X' when others;
	
	with asp_access select asp_idx <=
				asp_idx_init when 0,
				asp_idx_disp when 1,
				asp_idx_asp when 3,
				'X' when others;
	
	with asp_access select asp_row_write <=
				asp_row_write_init when 0,
				asp_row_write_asp when 3,
				'X' when others;
				
	with asp_access select asp_col_write <=
				'X' when others;
				
	-- display
	-- HEX0 and HEX1 always display contents of GRA
	disp0 <= 1 when gra_read(1) = '1' else 0;
	disp1 <= 1 when gra_read(0) = '1' else 0;
	
	with display_access select disp2 <=
				gra_col_disp when 1,
				gra_col_asp when 3,
				SW_col when others;
				
	with display_access select disp3 <=
				gra_row_disp when 1,
				gra_row_asp when 3,
				SW_row when others;
	
	-- ASP Next Move
	with start_incr select gra_write_asp <=
				gra_write_incr when '1',
				'X' when others;
				
	with start_incr select gra_row_asp <=
				gra_row_conf when '1',
				gra_row_incr when others;
				
	with start_incr select gra_col_asp <=
				gra_col_conf when '1',
				gra_col_incr when others;
				
	with start_incr select write_enable_gra_asp <=
				write_enable_gra_conf when '1',
				write_enable_gra_incr when others;
				
	with start_incr select asp_idx_asp <=
				inASP_longest when '1',
				asp_idx_incr when others;
				
	with start_incr select write_enable_asp_asp <=
				write_enable_asp_conf when '1',
				write_enable_asp_incr when others;
				
	-- LEDs light up when corresponding switches are high
	LEDR_row <= SW_row;
	LEDR_col <= SW_col;
	LEDR_gra_value <= SW_gra_value;

	controller : entity work.g29_controller
		port map(clock              => clock,
			     KEY_reset          => KEY_reset,
			     game_init          => game_init,
			     display            => display,
			     display_end        => display_end,
			     game_init_complete => game_init_complete,
			     player_ready       => player_ready,
			     NM                 => NM,
			     inASPMC            => inASPMC,
			     inSPMC             => inSPMC,
			     disk_ins           => disk_ins,
			     ASPMC              => ASPMC,
			     SPMC               => SPMC,
			     end_config         => end_config,
			     start_config       => start_config,
			     start_incr         => start_incr,
			     asp_access         => asp_access,
			     gra_access         => gra_access,
			     display_access     => display_access,
			     mv_count           => mv_count,
			     inASP_longest      => inASP_longest,
			     inASP_length       => inASP_length,
			     outASP_longest     => outASP_longest,
			     outASP_length      => outASP_length,
			     inASP_dr 		    => inASP_dr,
			     inASP_empty 		=> inASP_empty,
			     ASP_dr				=> ASP_dr,
			     ASP_empty			=> ASP_empty);
			     
	config : entity work.g29_row_config
		port map(clk              => clock,
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
			     
	increment : entity work.g29_row_incr
		port map(NM               => NM,
			     start_incr       => start_incr,
			     clk              => clock,
			     rst              => KEY_reset,
			     horiz            => horiz,
			     vertic           => vertic,
			     lr_diag          => lr_diag,
			     rl_diag          => rl_diag,
			     inASP_dr         => ASP_dr,
			     inASP_empty      => ASP_empty,
			     inASP_length     => outASP_length,
			     inASP_longest    => outASP_longest,
			     gra_read         => gra_read,
			     gra_write        => gra_write_incr,
			     gra_row          => gra_row_incr,
			     gra_col          => gra_col_incr,
			     write_enable_gra => write_enable_gra_incr,
			     asp_idx          => asp_idx_incr,
			     asp_row_read     => asp_row_read,
			     asp_col_read     => asp_col_read,
			     asp_row_write    => asp_row_write_incr,
			     asp_col_write    => asp_col_write_incr,
			     write_enable_asp => write_enable_asp_incr,
			     disk_ins         => disk_ins,
			     ASPMC            => inASPMC,
			     ASP_dr           => inASP_dr,
			     ASP_empty        => inASP_empty,
			     ASP_longest      => inASP_longest,
			     ASP_length       => inASP_length);
			     
	GRA : entity work.g29_gra_arr
		port map(row_in       => gra_row,
			     col_in       => gra_col,
			     gra_in       => gra_write,
			     write_enable => write_enable_gra,
			     clk          => clock,
			     rst          => KEY_reset,
			     gra_out      => gra_read);
			     
	ASP : entity work.g29_asp_registers
		port map(asp_idx      => asp_idx,
			     asp_row_in   => asp_row_write,
			     asp_col_in   => asp_col_write,
			     write_enable => write_enable_asp,
			     clk          => clock,
			     rst          => KEY_reset,
			     asp_row_out  => asp_row_read,
			     asp_col_out  => asp_col_read);
			     
	display : entity work.g29_display_asp
		port map(clock            => clock,
			     display          => display,
			     player_ready     => player_ready,
			     ASP_longest      => outASP_longest,
			     ASP_length       => outASP_length,
			     KEY_ready        => KEY_ready,
			     asp_idx          => asp_idx,
			     asp_row_read     => asp_row_read,
			     asp_col_read     => asp_col_read,
			     write_enable_asp => write_enable_asp,
			     write_enable_gra => write_enable_gra_disp,
			     gra_row		  => gra_row_disp,
			     gra_col		  => gra_col_disp);
			     
	sp_palyer : entity work.g29_sp_player
		port map(NM           => NM,
			     ASPMC        => ASPMC,
			     gra_read     => gra_read,
			     write_enable => write_enable_gra_sp,
			     gra_row      => gra_row_sp,
			     gra_col      => gra_col_sp,
			     gra_write    => gra_write_sp,
			     SW_row       => SW_row,
			     SW_col       => SW_col,
			     SW_gra_value => SW_gra_value,
			     KEY_write    => KEY_write,
			     clk          => clock,
			     rst          => KEY_reset,
			     mc_count     => mv_count,
			     SPMC         => inSPMC);
			     
	game_init : entity work.g29_game_init
		port map(clock              => clock,
			     reset              => KEY_reset,
			     game_init          => game_init,
			     write_enable_gra   => write_enable_gra_init,
			     gra_row            => gra_row_init,
			     gra_col            => gra_col_init,
			     gra_write          => gra_write_init,
			     asp_idx            => asp_idx_init,
			     asp_row_write      => asp_row_write_init,
			     asp_col_write      => asp_col_write_init,
			     write_enable_asp   => write_enable_asp_init,
			     SW_row             => SW_row,
			     SW_col             => SW_col,
			     KEY_write          => KEY_write,
			     game_init_complete => game_init_complete);
			     
	-- display GRA[r,c][1] on HEX0
	HEX0 : entity work.g29_7_segment_decoder
		port map(clk               => clock,
			     number            => disp1,
			     display_end       => display_end,
			     seven_seg_display => HEX0);
	
	-- display GRA[r,c][0] on HEX1		     
	HEX1 : entity work.g29_7_segment_decoder
		port map(clk               => clock,
			     number            => disp0,
			     display_end       => display_end,
			     seven_seg_display => HEX1);
	
	-- display selected column on HEX2		     
	HEX2 : entity work.g29_7_segment_decoder
		port map(clk               => clock,
			     number            => disp2,
			     display_end       => display_end,
			     seven_seg_display => HEX2);
	
	-- display selected row on HEX3		     
	HEX3 : entity work.g29_7_segment_decoder
		port map(clk               => clock,
			     number            => disp3,
			     display_end       => display_end,
			     seven_seg_display => HEX3);
			     

end everything;
