-- pwm_generator.vhd
-- Author: Fred
-- Status: Tested and passed
-- This module was based on the pwm module included here:
-- http://www.alteraforum.com/forum/showthread.php?t=45531

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;

entity pwm_generator is
	port (
		clock    			: in  std_logic                       := '0'; 			    		-- clock
		write_en_period     : in  std_logic                       := '0'; 			    		-- write_enable
		write_en_duty       : in  std_logic                       := '0'; 			    		-- write_enable
		period 				: in  std_logic_vector(15 downto 0)   := "0000000000000000";        -- period in clock ticks
		duty_count 			: in  std_logic_vector(15 downto 0)   := "0000000000000000";        -- duty must be less than or equal to period
		pwm_out 			: out std_logic                       := '0';              		  	-- output pwm signal
		reset    			: in  std_logic                       := '0'  						-- reset
	);
end entity pwm_generator;

architecture pwm of pwm_generator is

	signal period_count 	: std_logic_vector(15 downto 0) := "0000000000000000"; 

begin

	inc: process(clock, reset)
	begin
		if(reset = '1') then
			period_count <= "0000000000000000"; 
		elsif(rising_edge(clock)) then
			if(period_count < period) then
				period_count <= period_count + 1;
			else
				period_count <= "0000000000000000"; 
			end if;
		end if;
	end process;
	
	gen: process(period_count, duty_count)
	begin
		if(period_count < duty_count) then
			pwm_out <= '1';
		else
			pwm_out <= '0';
		end if;
	end process;

end architecture pwm; -- of pwm_generator

