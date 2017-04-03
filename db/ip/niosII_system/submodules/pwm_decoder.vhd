-- pwm_decoder.vhd
-- Author: Fred
-- Status: Tested and passed
-- Works with 490Hz Arduino PWM

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity pwm_decoder is
	port (
		clock    			: in		std_logic                       := '0'; 			    		-- clock
		pwm_in 				: in  		std_logic                       := '0';
		duty_count_out 		: out 		std_logic_vector(31 downto 0) 	:= "00000000000000000000000000000000";              		  	
		reset    			: in  		std_logic                       := '0'  						-- reset
	);
end entity pwm_decoder;

architecture decode of pwm_decoder is

begin

	inc: process(clock, reset)
	variable period 		: std_logic_vector(31 downto 0) := "00000000000000011000111010011000";
	variable duty_count 	: std_logic_vector(31 downto 0) := "00000000000000000000000000000000";
	variable count 			: std_logic_vector(31 downto 0) := "00000000000000000000000000000000";
	begin
		if(reset = '1') then
			duty_count := "00000000000000000000000000000000";
			count := "00000000000000000000000000000000";
		elsif(rising_edge(clock)) then
			
			if(unsigned(count) = unsigned(period) - 1) then
				duty_count_out <= duty_count;
				count := "00000000000000000000000000000000"; 
				duty_count := "00000000000000000000000000000000"; 
			else
				if(pwm_in = '1') then
					duty_count := std_logic_vector(unsigned(duty_count) + 1);
				end if;
				count := std_logic_vector(unsigned(count) + 1);
			end if;
		end if;
	end process;
	

end architecture decode; -- of pwm_decoder

