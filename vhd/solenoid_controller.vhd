-- solenoid_controller.vhd
-- Author: Fred
-- Status: Tested and passed

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity solenoid_controller is
	port (
		clock    			: in  std_logic                      := '0'; 			    -- clock
		status 				: out std_logic_vector(7 downto 0)   := "00000000";         -- read
		control 			: in  std_logic_vector(7 downto 0)   := "00000000";         -- write
		write_enable 		: in  std_logic                      := '0';                -- write enable
		btn_shift_up_in  	: in  std_logic                      := '0'; 				-- conduit_end.export
		btn_shift_down_in  	: in  std_logic                      := '0'; 				-- conduit_end.export
		sol_shift_up_out  	: out std_logic                      := '0'; 				-- conduit_end.export
		sol_shift_down_out  : out std_logic                      := '0'; 				-- conduit_end.export
		irq_btn_out  		: out std_logic                      := '0'; 				-- conduit_end.export
		reset    			: in  std_logic                      := '0'  				-- reset
	);
end entity solenoid_controller;

architecture ctrl of solenoid_controller is

	signal handling_btn 			: std_logic := '0'; 
	signal handling_control 		: std_logic := '0'; 
	signal tmp_control 			    : std_logic_vector(7 downto 0)   := "00000000";

begin

	process(clock, reset)
	begin
		if(reset = '1') then
			irq_btn_out <= '0';
			status <= "00000000";
			sol_shift_up_out <= '0';
			sol_shift_down_out <= '0';
			handling_btn <= '0';
			handling_control <= '0';
		elsif(rising_edge(clock)) then
			irq_btn_out <= '0';
			if(btn_shift_up_in = '1' and handling_btn = '0') then
				handling_btn <= '1';
				irq_btn_out <= '1';
				status <= "00000001";
			elsif (btn_shift_down_in = '1' and handling_btn = '0') then
				handling_btn <= '1';
				irq_btn_out <= '1';
				status <= "00000010";
			elsif(btn_shift_up_in = '0' and btn_shift_down_in = '0') then
				handling_btn <= '0';
				status <= "00000000";
			end if;
			tmp_control <= control;
			if(tmp_control = "00000000") then
				sol_shift_up_out <= '0';
				sol_shift_down_out <= '0';
			elsif(tmp_control = "00000001") then
				sol_shift_up_out <= '1';
				sol_shift_down_out <= '0';
			elsif(tmp_control = "00000010") then
				sol_shift_up_out <= '0';
				sol_shift_down_out <= '1';
			end if;

		end if;
	end process;

end architecture ctrl; -- of solenoid_controller

