----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 02.10.2017 11:44:55
-- Design Name: 
-- Module Name: INPUT_RAM - Behavioral
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

entity OUTPUT_RAM is
--  Port ( );
    port (
        clock : in std_logic;
        data_out : out Integer; -- from block RAM to PS
        data_out_read : out Integer; -- from block RAM to PL
        data_in : in Integer; -- from PL to block RAM
        address_in : in std_logic_vector(7 downto 0); -- control the index where the specific value is going to be saved
        address_out : in std_logic_vector(7 downto 0); -- control the index where the specific value is going to read
        mode : in std_logic;
        reset : in std_logic
    );
end OUTPUT_RAM;

architecture Behavioral of OUTPUT_RAM is
type RAM is array (0 to 35) of Integer;

signal DataMem: RAM := (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
signal second_output : integer := 0;
--signal DataMem: RAM := (others => (others => '0')); -- Initialize as 0 the entire memory block 
begin
    data_out_read <= second_output;
    process(clock)
    begin
        if rising_edge(clock) then
            if mode = '1' then 
                DataMem(to_integer(unsigned(address_in))) <= data_in; -- Synchronous write
                second_output <= DataMem(to_integer(unsigned(address_in))); -- Synchronous read when value is write.
            else
                data_out <= DataMem(to_integer(unsigned(address_out))); -- Synchronous read
            end if;
            
            if reset = '1' then
                DataMem <= (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
            end if;
--            if(to_integer(unsigned(address_in)) > 0) then
--                second_output <= DataMem(to_integer(unsigned(address_in)) + 1); -- Synchronous read when value is write.
--            else
--                second_output <= DataMem(to_integer(unsigned(address_in))); -- Synchronous read when value is write.
--            end if;
            --second_output <= DataMem(to_integer(unsigned(address_in)) + 1); -- Synchronous read when value is write.
            
        end if;
    end process;

end Behavioral;
