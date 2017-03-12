
-- This file is modified from lab1 top level vjdl file provided in eclass

-- A library clause declares a name as a library.  It 
-- does not create the library; it simply forward declares 
-- it. 
library ieee;

-- Commonly imported packages:

	-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
	use ieee.std_logic_1164.all;

	-- SIGNED and UNSIGNED types, and relevant functions
	use ieee.numeric_std.all;

	-- Basic sequential functions and concurrent procedures
	use ieee.VITAL_Primitives.all;
	
	use work.DE0_CONSTANTS.all;
	
	entity electronic_engine_control is
	
	port
	(
		-- Input ports and 50 MHz Clock
		KEY			: in  std_logic_vector (0 downto 0);
		SW			: in  std_logic_vector (0 downto 0);
		CLOCK_50	: in  std_logic;
		
		-- Green leds on board
		LED		: out DE0_LED_GREEN;
		
		-- SDRAM on board
		DRAM_ADDR	:	out	DE0_SDRAM_ADDR_BUS;
		DRAM_BA		:	out DE0_DRAM_BA;
		DRAM_CAS_N	:	out	std_logic;
		DRAM_CKE	:	out	std_logic;
		DRAM_CLK	:	out	std_logic;
		DRAM_CS_N	:	out	std_logic;
		DRAM_DQ		:	inout 	DE0_SDRAM_DATA_BUS;
		DRAM_DQM	:	out 	DE0_SDRAM_DQM;
		DRAM_RAS_N	: 	out	std_logic;
		DRAM_WE_N	: 	out std_logic;

		GPIO_0		:	out std_logic_vector (35 downto 0);

		GPIO_2		:	in std_logic_vector (7 downto 0);

		-- ADC
		ADC_CS_N	:	out std_logic;
		ADC_SADDR	:	out std_logic;
		ADC_SCLK	:	out	std_logic;
		ADC_SDAT	:	in	std_logic
		
	);
end electronic_engine_control;


architecture structure of electronic_engine_control is

	-- Declarations (optional)
	
	 component niosII_system is
        port (
            clk_0                                 : in    std_logic                     := 'X';             -- clk
            reset_n                               : in    std_logic                     := 'X';             -- reset_n
            sdram_0_wire_addr                     : out   DE0_SDRAM_ADDR_BUS;                               -- addr
            sdram_0_wire_ba                       : out   std_logic_vector(1 downto 0);                     -- ba
            sdram_0_wire_cas_n                    : out   std_logic;                                        -- cas_n
            sdram_0_wire_cke                      : out   std_logic;                                        -- cke
            sdram_0_wire_cs_n                     : out   std_logic;                                        -- cs_n
            sdram_0_wire_dq                       : inout DE0_SDRAM_DATA_BUS := (others => 'X');            -- dq
            sdram_0_wire_dqm                      : out   std_logic_vector(1 downto 0);                     -- dqm
            sdram_0_wire_ras_n                    : out   std_logic;                                        -- ras_n
            sdram_0_wire_we_n                     : out   std_logic;                                        -- we_n
            altpll_0_c0_out                       : out   std_logic;                                        -- clk
            out_port_from_the_green_leds          : out   DE0_LED_GREEN;                                    -- led out 
            in_port_to_the_switch                 : in    std_logic                     := 'X';             -- switch in 
			adc_sclk_from_the_de0_nano_adc_0      : out   std_logic;                                        -- adc_sclk
            adc_cs_n_from_the_de0_nano_adc_0      : out   std_logic;                                        -- adc_cs_n
            adc_dout_to_the_de0_nano_adc_0        : in    std_logic                     := 'X';             -- adc_dout
            adc_din_from_the_de0_nano_adc_0       : out   std_logic;      						            -- adc_din
			pwm_generator_0_pwm_out_export        : out   std_logic                     := '0';             -- pwm_out
            rs232_0_external_interface_RXD                       : in    std_logic      := 'X';             -- RXD
            rs232_0_external_interface_TXD                       : out   std_logic;                          -- TXD
			solenoid_out_external_connection_export 			 : out   std_logic_vector(7 downto 0)  := "00000000"; -- export solenid drivin pins
            buttons_external_connection_export                   : in    std_logic_vector(7 downto 0)  := "00000000"  -- export buttons
        );
    end component niosII_system;

--	These signals are for matching the provided IP core to
--  The specific SDRAM chip in our system	 
	 signal BA	:   std_logic_vector (1 downto 0);
	 signal DQM	:	std_logic_vector (1 downto 0);
	 

begin

	DRAM_BA(1) <= BA(1);
	DRAM_BA(0) <= BA(0);

	DRAM_DQM(1) <= DQM(1);
	DRAM_DQM(0) <= DQM(0);
	
	-- Component Instantiation Statement (optional)
	
	  u0 : component niosII_system
        port map (
            clk_0                                => CLOCK_50,                                
            reset_n                              => KEY(0),                          
            sdram_0_wire_addr                    => DRAM_ADDR,                      
            sdram_0_wire_ba                      => BA,                        
            sdram_0_wire_cas_n                   => DRAM_CAS_N,                      
            sdram_0_wire_cke                     => DRAM_CKE,                       
            sdram_0_wire_cs_n                    => DRAM_CS_N,                      
            sdram_0_wire_dq                      => DRAM_DQ,                         
            sdram_0_wire_dqm                     => DQM,                        
            sdram_0_wire_ras_n                   => DRAM_RAS_N,                     
            sdram_0_wire_we_n                    => DRAM_WE_N,                       
            altpll_0_c0_out                      => DRAM_CLK,                        
            out_port_from_the_green_leds         => LED,  
            in_port_to_the_switch                => SW(0),                  
			adc_sclk_from_the_de0_nano_adc_0     => ADC_SCLK,
			adc_cs_n_from_the_de0_nano_adc_0     => ADC_CS_N,
			adc_dout_to_the_de0_nano_adc_0       => ADC_SDAT,
			adc_din_from_the_de0_nano_adc_0      => ADC_SADDR,
			pwm_generator_0_pwm_out_export		 => GPIO_0(0),
			solenoid_out_external_connection_export(2)				=> GPIO_0(2),
			solenoid_out_external_connection_export(3)				=> GPIO_0(3),
			buttons_external_connection_export(2)  			   		=> GPIO_2(2),
			buttons_external_connection_export(3)  			   		=> GPIO_2(3),
			rs232_0_external_interface_TXD							=> GPIO_0(7),
			rs232_0_external_interface_RXD							=> GPIO_2(7)
        );

end structure;


library ieee;

-- Commonly imported packages:

	-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
	use ieee.std_logic_1164.all;

package DE0_CONSTANTS is
	
	type DE0_SDRAM_ADDR_BUS is array(11 downto 0) of std_logic;
	type DE0_SDRAM_DATA_BUS is array(15 downto 0) of std_logic;
	type DE0_SDRAM_DQM is array(1 downto 0) of std_logic;
	
	type DE0_LED_GREEN		is array(7 downto 0)  of std_logic;

	type DE0_DRAM_BA is array(1 downto 0) of std_logic;
	
end DE0_CONSTANTS;



