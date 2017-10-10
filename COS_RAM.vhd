----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 26.09.2017 22:17:04
-- Design Name: 
-- Module Name: COS_RAM - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity COS_RAM is
--  Port ( );
    Port(
        Address : in STD_LOGIC_VECTOR(7 downto 0);
        clock : in STD_LOGIC;
        output_data : out INTEGER;
        table_select : in STD_LOGIC_VECTOR(1 downto 0)
    );
end COS_RAM;

architecture Behavioral of COS_RAM is
type ROM is array (0 to 143) of integer;
constant GET_COS_ROM: ROM := 
    (
    1000, 999, 996, 991, 985, 976, 966, 954, 940, 924, 
    906, 887, 866, 843, 819, 793, 766, 737, 707, 676, 
    643, 609, 574, 537, 500, 462, 423, 383, 342, 301, 
    259, 216, 174, 131, 87, 44, 0, -44, -87, -131, 
    -174, -216, -259, -301, -342, -383, -423, -462, -500, -537, 
    -574, -609, -643, -676, -707, -737, -766, -793, -819, -843, 
    -866, -887, -906, -924, -940, -954, -966, -976, -985, -991, 
    -996, -999, -1000, -999, -996, -991, -985, -976, -966, -954, 
    -940, -924, -906, -887, -866, -843, -819, -793, -766, -737, 
    -707, -676, -643, -609, -574, -537, -500, -462, -423, -383, 
    -342, -301, -259, -216, -174, -131, -87, -44, 0, 44, 
    87, 131, 174, 216, 259, 301, 342, 383, 423, 462, 
    500, 537, 574, 609, 643, 676, 707, 737, 766, 793, 
    819, 843, 866, 887, 906, 924, 940, 954, 966, 976, 
    985, 991, 996, 999
    );
begin
    process(clock)
    begin
        if rising_edge(clock) then
            if (table_select = b"00") then
                output_data <= GET_COS_ROM(to_integer(unsigned(Address)));
            end if; 
        end if;
    end process;
--output_data <= GET_COS_ROM(to_integer(unsigned(Address)));  -- read from COS table (asynchronous reads)
end Behavioral;
