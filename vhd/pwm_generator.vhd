-- pwm_generator.vhd
-- Author: Fred
-- Status: Tested and passed
-- This module was based on the pwm module included here:
-- http://www.alteraforum.com/forum/showthread.php?t=45531

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity pwm_generator is
	port (
		clock    			: in		std_logic                       := '0'; 			    		-- clock
		write_en_period   : in  	std_logic                       := '0'; 			    		-- write_enable
		write_en_duty     : in  	std_logic                       := '0'; 			    		-- write_enable
		period 				: in  	std_logic_vector(31 downto 0)   := "00000000000000000000000000000000";        -- period in clock ticks
		duty_count 			: in  	std_logic_vector(31 downto 0)   := "00000000000000000000000000000000";        -- duty must be less than or equal to period
		pwm_out 				: out 	std_logic                       := '0';              		  	-- output pwm signal
		reset    			: in  	std_logic                       := '0'  						-- reset
	);
end entity pwm_generator;

architecture pwm of pwm_generator is

	signal curr_period 		: std_logic_vector(31 downto 0) := "00000000000000000000100000000000"; 

	signal curr_duty_count 	: std_logic_vector(31 downto 0) := "00000000000000000000000000000000"; 

	signal period_count 		: std_logic_vector(31 downto 0) := "00000000000000000000000000000000"; 

begin

	inc: process(clock, reset)
	begin
		if(reset = '1') then
			period_count <= "00000000000000000000000000000000"; 
		elsif(rising_edge(clock)) then

			if(write_en_period = '1') then
				curr_period <= period;
				period_count <= "00000000000000000000000000000000"; 
			end if;

			if(unsigned(period_count)  =  unsigned(curr_period) - 1) then
				period_count <= "00000000000000000000000000000000"; 
			else
				period_count <= std_logic_vector( unsigned(period_count) + 1 );
			end if;

			if(write_en_duty = '1') then
				curr_duty_count <= duty_count;
				period_count <= "00000000000000000000000000000000"; 
			end if;

			if(unsigned(period_count) < unsigned(curr_duty_count)) then
				pwm_out <= '1';
			else
				pwm_out <= '0';
			end if;

		end if;
	end process;
	

end architecture pwm; -- of pwm_generator

