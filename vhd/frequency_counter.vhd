-- frequency_counter.vhd

-- This file was auto-generated as a prototype implementation of a module
-- created in component editor.  It ties off all outputs to ground and
-- ignores all inputs.  It needs to be edited to make it do something
-- useful.
-- 
-- This file will not be automatically regenerated.  You should check it in
-- to your version control system if you want to keep it.

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity frequency_counter is
	generic (
		AUTO_CLOCK_CLOCK_RATE : string := "-1"
	);
	port (
		clock    : in  std_logic                     := '0'; --        clock.clk
		data_out : out std_logic_vector(31 downto 0) := "00000000000000000000000000000000";        -- avalon_slave.readdata
		freq_in  : in  std_logic                     := '0'; --  conduit_end.export
		reset    : in  std_logic                     := '0'  --        reset.reset_n
	);
end entity frequency_counter;

architecture rtl of frequency_counter is

	signal count : std_logic_vector(31 downto 0) := "00000000000000000000000000000000";  

begin

	process(freq_in)
	begin
		if(freq_in = '1') then
			count <= std_logic_vector( unsigned(count) + 1 );
			data_out <= count;
		end if;
	end process;

end architecture rtl; -- of frequency_counter

