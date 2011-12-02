-- determines next ASP move if longest row is not dead or empty
-- 
-- entity name: g29_row_incr_config
--
-- Version 1.1
-- Author: Stepan Salenikovich, Ayman Zakir
-- Date: 25.11.2011

library ieee; -- allows use of the std_logic_vector type
use ieee.std_logic_1164.all;
use ieee.numeric_std.all; -- functions for converting between std_logic and int

use work.types.all; --package with types and relevant functions

entity g29_row_incr is
	port(
		-- INPUTS --
		NM: in std_logic; --Next Move, active low
		start_incr: in std_logic; -- when configuration determined
		clk, rst: in std_logic; -- clock and reset
		horiz, vertic, lr_diag, rl_diag: in std_logic; -- configuration of longest row
		
		inASP_dr: in std_logic_vector(0 to 7); -- dead row register
		inASP_empty: in std_logic_vector(0 to 7); -- empty row register
		inASP_length: in natural range 0 to 4; -- length of longest row
		inASP_longest: in natural range 0 to 7; -- index of the longest ASP register
		
		-- GRA
		gra_read: in std_logic_vector(0 to 1);
		gra_write: out std_logic_vector(0 to 1);
		gra_row: out natural range 0 to 7; -- row index
		gra_col: out natural range 0 to 7; -- column index
		write_enable_gra : out std_logic;  -- active low
		-- ASP
		asp_idx: out natural range 0 to 7;  -- index of desired ASP register
		asp_row_read, asp_col_read: in ASP_register;  -- ASP row and col read from ASP Registers Module
		asp_row_write, asp_col_write: out ASP_register;  -- ASP row and col to be wirtten
		write_enable_asp: out std_logic;  -- active low
		
		-- OUTPUTS --
		disk_ins: out std_logic;
		ASPMC: out std_logic;
		ASP_dr: out std_logic_vector(0 to 7);
		ASP_empty: out std_logic_vector(0 to 7);
		ASP_longest: out natural range 0 to 7;
		ASP_length: out natural range 0 to 4
);
end g29_row_incr;


architecture logic of g29_row_incr is
	type state_type is (fetch, config1, config2, insertion1, insertion2, random1, random2, last);
	signal state : state_type;
begin

	process(clk, rst)
		
		variable reg_idx : natural range 0 to 3 := 0; -- index of int in ASP register
		variable row_0, col_0, row_last, col_last, row_new, col_new: natural range 0 to 7;
		variable inser_flag : std_logic := '0'; -- disk insertion flag; temp var for disk_ins out signal; init to 0
		variable dr_temp, empty_temp: std_logic_vector(0 to 7);  -- temp dead and empty row var
		variable empty_counter: natural range 0 to 8 := 0; -- count empty rows

	begin
		if rst = '0' then
			state <= fetch;
			disk_ins <= '1';
			inser_flag := '1';
			write_enable_gra <= '1';
			write_enable_asp <= '1';
			ASPMC <= '1';
			ASP_longest <= 0;
			ASP_length <= 0;
		elsif rising_edge(clk) then
				
			--row_asp := to_integer( unsigned(asp_row_read));
			--col_asp := to_integer( unsigned(asp_col_read));
			
	
			if NM = '0' and start_incr = '0' then
			
				if state = fetch then
				
					ASPMC <= '1';
					inser_flag := '1';
					disk_ins <= '1';
					write_enable_gra <= '1';
					write_enable_asp <= '1';
					
					empty_temp := inASP_empty;
					dr_temp := inASP_dr;
	
					asp_idx <= inASP_longest;
					
					ASP_longest <= inASP_longest;
					ASP_length <= inASP_length;
					
					-- check if there is an empty ASP array left
					empty_counter := 0;
					for ASP_idx in 0 to 7 loop	
						if empty_temp(ASP_idx) = '0' then
							empty_counter := empty_counter + 1;
						end if;	
					end loop;
			
					-- if row configuration know, try to extend config
					-- else go to random algo for determining next move
					if horiz = '1' or vertic = '1' or lr_diag = '1' or rl_diag = '1' then
						state <= config1;
					else
						row_new := 0;
						col_new := 0;
						gra_row <= row_new;
						gra_col <= col_new;
						state <= random1;
					end if;
				
				elsif state = config1 then
					write_enable_gra <= '1';
					write_enable_gra <= '1';
					
					row_0 := asp_row_read(0);
					col_0 := asp_col_read(0);
					row_last := asp_row_read(inASP_length - 1);
					col_last := asp_col_read(inASP_length - 1);
	
					if horiz = '1' then
						-- try to the left
						if col_0 /= 0 then
							row_new := row_0;
							col_new := col_0 - 1;
							state <= insertion1;
						-- else to the right
						else
							state <= config2;
						end if;
					elsif vertic = '1' then 
						-- try below
						if row_0 /= 0 then
							row_new := row_0 - 1;
							col_new := col_0;
							state <= insertion1;
						-- else above
						else
							state <= config2;
						end if;
					elsif lr_diag = '1' then
						-- try bottom right
						if row_0 /= 0 and col_0 /= 6 then
							row_new := row_0 - 1;
							col_new := col_0 + 1;
							state <= insertion1;
						-- else top left
						else
							state <= config2;
						end if;
					elsif rl_diag = '1' then
						-- try bottom left
						if row_0 /= 0 and col_0 /= 0 then
							row_new := row_0 - 1;
							col_new := col_0 - 1;
							state <= insertion1;
						-- else top right
						else
							state <= config2;
						end if;
					end if;	
					-- fetch gra value	
					gra_row <= row_new;
					gra_col <= col_new;
					
				elsif state = config2 then
					write_enable_gra <= '1';
					write_enable_gra <= '1';
	
					if horiz = '1' then
						if col_last /= 6 then
							row_new := row_0;
							col_new := col_last + 1;
							state <= insertion2;
						else
							-- can't go anywhere
							row_new := 0;
							col_new := 0;
							state <= random1;
						end if;
					elsif vertic = '1' then 
						if row_last /= 5 then
							row_new := row_last + 1;
							col_new := col_0;
							state <= insertion2;
						else
							-- can't go anywhere
							row_new := 0;
							col_new := 0;
							state <= random1;
						end if;
					elsif lr_diag = '1' then
						if row_last /= 5 and col_last /= 0 then
							row_new := row_last + 1;
							col_new := col_last - 1;
							state <= insertion2;
						else
							-- can't go anywhere
							row_new := 0;
							col_new := 0;
							state <= random1;
						end if;
					elsif rl_diag = '1' then
						if row_last /= 5 and col_last /= 6 then
							row_new := row_last + 1;
							col_new := col_last + 1;
							state <= insertion2;
						else
							-- can't go anywhere
							row_new := 0;
							col_new := 0;
							state <= random1;
						end if;
					end if;	
					-- fetch gra value	
					gra_row <= row_new;
					gra_col <= col_new;
					
				elsif state = insertion1 or state = insertion2 then
					-- if cell is empty, raise disk insertion flag
					-- and add cell coordinates to the begining of the ASP register
					if gra_read = "00" then
						inser_flag := '0';
						
						--write to gra
						write_enable_gra <= '0';
						gra_row <= row_new;
						gra_col <= col_new;
						gra_write <= "01";
						
						write_enable_asp <= '0';
						if state = insertion1 then
							-- shift ASP register right and insert new cell coordinates --
							for reg_idx in 0 to 2 loop
								-- take previous column and put it into the next one
								-- (3 - reg_inx because VHDL doesn't allow for downcounting for loops)
								asp_row_write(3 - reg_idx) <= asp_row_read(2 - reg_idx);
								asp_col_write(3 - reg_idx) <= asp_col_read(2 - reg_idx);
							end loop;
							-- write new value to 0th ASP entry
							asp_row_write(0) <= row_new;
							asp_col_write(0) <= col_new;
						else
							-- write to end of ASP
							for reg_idx in 0 to 3 loop
								if reg_idx /= inASP_length then
									asp_row_write(reg_idx) <= asp_row_read(reg_idx);
									asp_col_write(reg_idx) <= asp_col_read(reg_idx);
								else
									asp_row_write(inASP_length) <= row_new;
									asp_col_write(inASP_length) <= col_new;
								end if;
							end loop;
						end if;
						
						ASP_length <= inASP_length + 1;
						
						state <= last;
						
					elsif state = insertion1 then
					-- trying going the other way
						state <= config2;
					elsif state = insertion2 then
					-- cant insert into longest ASP
						write_enable_gra <= '1';
						write_enable_asp <= '1';
						inser_flag := '1';
						
						row_new := 0;
						col_new := 0;
						gra_row <= row_new;
						gra_col <= row_new;
						state <= random1;
					end if;			
				
				--|------------------------|--
				--|--------ALL-EMPTY-------|--
				--|-----------OR-----------|--
				--|--------DEAD-ROW--------|--
				--|------------------------|--
				
				elsif state = random1 or state = random2 then
					if state = random1 then
						-- mark ASP register as dead row if it was not of length 0
						dr_temp := inASP_dr;
						
						if inASP_length /= 0 then
							dr_temp(inASP_longest) := '1';
						end if;					
					end if;
					
					if state = random2 or empty_counter /= 8 then
						-- starting from GRA[0,0] find first empty cell
						-- check cell if its empty
						if gra_read = "00" then
							inser_flag := '0';
							
							-- write to gra
							write_enable_gra <= '0';
							gra_row <= row_new;
							gra_col <= col_new;
							gra_write <= "01";
							
							-- write to end of ASP
							write_enable_asp <= '0';
							asp_idx <= empty_counter;
							asp_row_write <= (0 => row_new, others=> 7);
							asp_col_write <= (0 => col_new, others=> 7);
							
							-- mark this ASP as not empty 
							empty_temp(empty_counter) := '0';
							
							ASP_longest <= empty_counter;
							ASP_length <= 1;
							
							state <= last;
						-- while cells in GRA still left
						elsif not (row_new >= 5 and col_new >= 6) then
							write_enable_asp <= '1';
							
							if col_new < 6 then
								col_new := col_new + 1;
							else
								col_new := 0;
								row_new := row_new + 1;
							end if;
							
							-- fetch next gra value
							write_enable_gra <= '1';
							gra_row <= row_new;
							gra_col <= col_new;
							state <= random2;
						else
							write_enable_asp <= '1';
							write_enable_gra <= '1';
							-- no empty cells
							inser_flag := '1';
							state <= last;
						end if;
					else
						-- nothing can be done
						inser_flag := '1';
						state <= last;
					end if;
				
				-- LAST!
				elsif state = last then
					write_enable_gra <= '1';
					write_enable_asp <= '1';
				
					-- set ins_disk to inser_flag
					disk_ins <= inser_flag;
					ASPMC <= '0';
					
					-- set ASP stuff
					asp_idx <= empty_counter;  -- this is the current longest ASP
					
					ASP_empty <= empty_temp;
					ASP_dr <= dr_temp;
					state <= fetch;
				end if;
			end if;
		end if;	
	end process;
end logic;