-------------------------------------------------------------------------------
-- mb_system_top.vhd
-------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library UNISIM;
use UNISIM.VCOMPONENTS.ALL;

entity mb_system_top is
  port (
    rzq : inout std_logic;
    mcbx_dram_we_n : out std_logic;
    mcbx_dram_udqs : inout std_logic;
    mcbx_dram_udm : out std_logic;
    mcbx_dram_ras_n : out std_logic;
    mcbx_dram_ldm : out std_logic;
    mcbx_dram_dqs : inout std_logic;
    mcbx_dram_dq : inout std_logic_vector(15 downto 0);
    mcbx_dram_clk_n : out std_logic;
    mcbx_dram_clk : out std_logic;
    mcbx_dram_cke : out std_logic;
    mcbx_dram_cas_n : out std_logic;
    mcbx_dram_ba : out std_logic_vector(1 downto 0);
    mcbx_dram_addr : out std_logic_vector(12 downto 0);
    USB_Uart_sout : out std_logic;
    USB_Uart_sin : in std_logic;
    RESET : in std_logic;
    CDCE913_I2C_Sda_pin : inout std_logic;
    CDCE913_I2C_Scl_pin : inout std_logic
    --CLK_50MHZ : in std_logic
  );
end mb_system_top;

architecture STRUCTURE of mb_system_top is

  component mb_system is
    port (
      rzq : inout std_logic;
      mcbx_dram_we_n : out std_logic;
      mcbx_dram_udqs : inout std_logic;
      mcbx_dram_udm : out std_logic;
      mcbx_dram_ras_n : out std_logic;
      mcbx_dram_ldm : out std_logic;
      mcbx_dram_dqs : inout std_logic;
      mcbx_dram_dq : inout std_logic_vector(15 downto 0);
      mcbx_dram_clk_n : out std_logic;
      mcbx_dram_clk : out std_logic;
      mcbx_dram_cke : out std_logic;
      mcbx_dram_cas_n : out std_logic;
      mcbx_dram_ba : out std_logic_vector(1 downto 0);
      mcbx_dram_addr : out std_logic_vector(12 downto 0);
      USB_Uart_sout : out std_logic;
      USB_Uart_sin : in std_logic;
      RESET : in std_logic;
      CDCE913_I2C_Sda_pin : inout std_logic;
      CDCE913_I2C_Scl_pin : inout std_logic;
      CLK_50MHZ : in std_logic
    );
  end component;

 attribute BOX_TYPE : STRING;
 attribute BOX_TYPE of mb_system : component is "user_black_box";

 signal CFGMCLK : std_logic;
 signal CLK_50MHZ : std_logic;

begin

  mb_system_i : mb_system
    port map (
      rzq => rzq,
      mcbx_dram_we_n => mcbx_dram_we_n,
      mcbx_dram_udqs => mcbx_dram_udqs,
      mcbx_dram_udm => mcbx_dram_udm,
      mcbx_dram_ras_n => mcbx_dram_ras_n,
      mcbx_dram_ldm => mcbx_dram_ldm,
      mcbx_dram_dqs => mcbx_dram_dqs,
      mcbx_dram_dq => mcbx_dram_dq,
      mcbx_dram_clk_n => mcbx_dram_clk_n,
      mcbx_dram_clk => mcbx_dram_clk,
      mcbx_dram_cke => mcbx_dram_cke,
      mcbx_dram_cas_n => mcbx_dram_cas_n,
      mcbx_dram_ba => mcbx_dram_ba,
      mcbx_dram_addr => mcbx_dram_addr,
      USB_Uart_sout => USB_Uart_sout,
      USB_Uart_sin => USB_Uart_sin,
      RESET => RESET,
      CDCE913_I2C_Sda_pin => CDCE913_I2C_Sda_pin,
      CDCE913_I2C_Scl_pin => CDCE913_I2C_Scl_pin,
      CLK_50MHZ => CLK_50MHZ
    );

  startup_spartan6_i : startup_spartan6
    port map (
		CFGCLK => open, -- Configuration logic main clock output
		CFGMCLK => CFGMCLK, -- Configuration internal oscillator clock output
		EOS => open, -- Active high output signal indicate the End Of Configuration
		CLK => '0', -- User startup-clock input
		GSR => '0', -- Global Set/Reset input (GSR cannot be used for the port name)
		GTS => '0', -- Global 3-state input (GTS cannot be used for the port name)
		KEYCLEARB => '0' -- Clear AES Decrypter Key input from Battery-Backed RAM(BBRAM)
	);

  bufgmux_i : bufgmux
    port map (
		O => CLK_50MHZ, -- Clock buffer output
		I0 => CFGMCLK, -- Clock buffer input (S=0)
		I1 => CFGMCLK, -- Clock buffer input (S=1)
		S => '0' -- Clock buffer select
	);

end architecture STRUCTURE;

