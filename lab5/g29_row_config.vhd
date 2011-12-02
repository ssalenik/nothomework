-- decides on the configuration of a row with only one entry
--
-- Version 1.1
-- Author: Stepan Salenikovich, Ayman Zakir
-- Date: 02.12.2011

library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;

use work.types.all; --package with types

entity g29_row_config is
	port ( 
		-- input
		clk: in std_logic;
		NM: in std_logic;
		start_config: in std_logic;
		inASP_length: in natural range 0 to 4;
		-- ASP
		asp_row_read, asp_col_read: in ASP_register;
		write_enable_asp: out std_logic;  -- active low
		-- GRA
		gra_read: in std_logic_vector(0 to 1);
		gra_row: out natural range 0 to 7; -- row index
		gra_col: out natural range 0 to 7; -- column index
		write_enable_gra : out std_logic;  -- active low
		-- output
		horiz, vertic, lr_diag, rl_diag: out std_logic;
		end_config: out std_logic);
end g29_row_config;

architecture logic of g29_row_config is
	-- L = left, R = right, D = down, U = up
	-- [L, L-D, L-U, R, R-D, R-U, D, U]\
	signal direction : natural range 0 to 7 := 0;
	signal fetch_gra : std_logic := '1'; -- activle low
	constant read : std_logic := '1';
begin
	process(clk)
	begin
		if rising_edge(clk) then
			if start_config = '0' then
				if inASP_length = 0 then
					horiz <= '0';
					vertic <= '0';
					lr_diag <= '0';
					rl_diag <= '0';
				elsif inASP_length = 1 then
					-- check GRA for plausible configurations
					if fetch_gra = '1' then
						if direction = 0 then
							--left
							if asp_col_read(0) > 0 then
								gra_row <= asp_row_read(0);
								gra_col <= asp_col_read(0) - 1;
								fetch_gra <= '0';
							else
								-- try another direction
								direction <= 1;
								fetch_gra <= '1';
							end if;
						elsif direction = 1 then
							--left down
							if asp_col_read(0) > 0 and asp_row_read(0) > 0 then
								gra_row <= asp_row_read(0) - 1;
								gra_col <= asp_col_read(0) - 1;
								fetch_gra <= '0';
							else
								-- try another direction
								direction <= 2;
								fetch_gra <= '1';
							end if;
						elsif direction = 2 then
							--left up
							if asp_col_read(0) > 0 and asp_row_read(0) < 5 then
								gra_row <= asp_row_read(0) + 1;
								gra_col <= asp_col_read(0) - 1;
								fetch_gra <= '0';
							else
								-- try another direction
								direction <= 3;
								fetch_gra <= '1';
							end if;
						elsif direction = 3 then
							--right
							if asp_col_read(0) < 6 then
								gra_row <= asp_row_read(0);
								gra_col <= asp_col_read(0) + 1;
								fetch_gra <= '0';
							else
								-- try another direction
								direction <= 4;
								fetch_gra <= '1';
							end if;
						elsif direction = 4 then
							--right down
							if asp_col_read(0) < 6 and asp_row_read(0) > 0 then
								gra_row <= asp_row_read(0) - 1;
								gra_col <= asp_col_read(0) + 1;
								fetch_gra <= '0';
							else
								-- try another direction
								direction <= 5;
								fetch_gra <= '1';
							end if;
						elsif direction = 5 then
							--right up
							if asp_col_read(0) < 6 and asp_row_read(0) < 5 then
								gra_row <= asp_row_read(0) + 1;
								gra_col <= asp_col_read(0) + 1;
								fetch_gra <= '0';
							else
								-- try another direction
								direction <= 6;
								fetch_gra <= '1';
							end if;
						elsif direction = 6 then
							-- down
							if asp_row_read(0) > 0 then
								gra_row <= asp_row_read(0) - 1;
								gra_col <= asp_col_read(0);
								fetch_gra <= '0';
							else
								-- try another direction
								direction <= 7;
								fetch_gra <= '1';
							end if;
						elsif direction = 7 then
							-- up
							if asp_row_read(0) < 6 then
								gra_row <= asp_row_read(0) + 1;
								gra_col <= asp_col_read(0);
								fetch_gra <= '0';
							else
								-- no more directions
								direction <= 0;
								fetch_gra <= '1';
								end_config <= '0';
								horiz <= '1';
								vertic <= '0';
								lr_diag <= '0';
								rl_diag <= '0';
							end if;
						end if;
					else
						if gra_read = "00" then
							if direction = 0 or direction = 3 then
								horiz <= '1';
								vertic <= '0';
								lr_diag <= '0';
								rl_diag <= '0';
							elsif direction = 6 or direction = 7 then
								horiz <= '0';
								vertic <= '1';
								lr_diag <= '0';
								rl_diag <= '0';
							elsif direction = 2 or direction = 4 then
								horiz <= '0';
								vertic <= '0';
								lr_diag <= '1';
								rl_diag <= '0';
							elsif direction = 1 or direction = 5 then
								horiz <= '0';
								vertic <= '0';
								lr_diag <= '0';
								rl_diag <= '1';
							end if;
							end_config <= '0';
						else
							direction <= direction + 1;
						end if;
						fetch_gra <= '1';
					end if;	
				elsif inASP_length > 1 then
					if asp_row_read(0) = asp_row_read(1) and asp_col_read(0) < asp_col_read(1) then
						horiz <= '1';
						vertic <= '0';
						lr_diag <= '0';
						rl_diag <= '0';
					elsif asp_row_read(0) < asp_row_read(1) and asp_col_read(0) = asp_col_read(1) then
						horiz <= '0';
						vertic <= '1';
						lr_diag <= '0';
						rl_diag <= '0';
					elsif asp_row_read(0) < asp_row_read(1) and asp_col_read(0) > asp_col_read(1) then
						horiz <= '0';
						vertic <= '0';
						lr_diag <= '1';
						rl_diag <= '0';
					elsif asp_row_read(0) < asp_row_read(1) and asp_col_read(0) < asp_col_read(1) then
						horiz <= '0';
						vertic <= '0';
						lr_diag <= '0';
						rl_diag <= '1';
					else
						horiz <= '0';
						vertic <= '0';
						lr_diag <= '0';
						rl_diag <= '0';
					end if;
				end if;
			end if;
		end if;
	end;
	
	-- always reading
	write_enable_gra <= read;
	write_enable_asp <= read;

end logic;