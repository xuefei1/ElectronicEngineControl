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
		write_en_period   	: in  		std_logic                       := '0'; 			    		-- write_enable_period
		write_en_duty     	: in  		std_logic                       := '0'; 			    		-- write_enable_duty
		write_en_control	: in  		std_logic                       := '0'; 			    		-- write_enable_control
		period_in 			: in  		std_logic_vector(31 downto 0)   := "00000000000000000000000000000000";        -- period in clock ticks
		duty_in 			: in  		std_logic_vector(31 downto 0)   := "00000000000000000000000000000000";        -- duty must be less than or equal to period
		control_in 			: in  		std_logic_vector(7 downto 0)    := "00000000";        						  -- control input(bit 0: output enable)
		pwm_out 			: out 		std_logic                       := '0';              		  	-- output pwm signal
		reset    			: in  		std_logic                       := '0'  						-- reset
	);
end entity pwm_generator;

architecture pwm of pwm_generator is

begin

	inc: process(clock, reset)
	variable period 		: std_logic_vector(31 downto 0) := "00000000000000000000000000000000";
	variable duty_count 	: std_logic_vector(31 downto 0) := "00000000000000000000000000000000";
	variable count 			: std_logic_vector(31 downto 0) := "00000000000000000000000000000000";
	variable control	: std_logic_vector(7 downto 0) := "00000000";
	begin
		if(reset = '1') then
			period := "00000000000000000000000000000000"; 
			duty_count := "00000000000000000000000000000000";
			control := "00000000";
		elsif(rising_edge(clock)) then

			if(write_en_period = '1') then
				period := period_in;
			end if;

			if(write_en_duty = '1') then
				duty_count := duty_in; 
				if(duty_count > period) then
					duty_count := period;
				end if;
			end if;
			
			if(write_en_control = '1') then
				control := control_in; 
			end if;
			
			if(unsigned(count) = unsigned(period) - 1) then
				count := "00000000000000000000000000000000"; 
			else
				count := std_logic_vector(unsigned(count) + 1);
			end if;

			if(unsigned(count) < unsigned(duty_count)) then
				pwm_out <= '1';
			else
				pwm_out <= '0';
			end if;
		end if;
	end process;
	

end architecture pwm; -- of pwm_generator

