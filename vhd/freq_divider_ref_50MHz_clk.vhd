-- freq_16kHz_clk.vhd
-- Author: Fred
-- Status: Tested and passed

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity freq_divider_ref_50MHz_clk is
	port (
		clock_in    			: in		std_logic                       := '0'; 			    		-- 50 MHz clk in
		clock_out				: out		std_logic						:= '0';							-- 16 kHz clk out
		write_enable			: in		std_logic						:= '0';	
		freq_in					: in		std_logic_vector(31 downto 0)   := "00000000000000000000000000000000"; -- in terms of # of 50MHz clk in
		reset					: in		std_logic						:= '0'							-- reset
	);
end entity freq_divider_ref_50MHz_clk;

architecture clk_out of freq_divider_ref_50MHz_clk is

signal clk_out : std_logic := '0';
	
begin
	inc: process(clock, reset)
	variable limit : integer range 0 to 4294967295;
	variable count : integer range 0 to 4294967295;
	begin
		if(reset = '1') then
			limit := 0; -- 50MHz -> 20ns period, for example, 16kHz -> 62500ns period, so every 3125 clk in, we flip clk out
			count := 0;
		elsif(rising_edge(clock)) then
		
			if(write_enable = '1') then
				limit := unsigned(freq_in);
				count := 0;
			end if;
			
			if(count = limit - 1) then
				clock_out <= NOT(clk_out);
				count := 0;
			else
				count := count + 1;
			end if;

		end if;
	end process;
	

end architecture clk_out;

