// RAMAddress.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <fstream>
#include  <iomanip>

int main()
{
	const uint32_t MAX_ADDRESS = 1 << 20;
	const uint32_t MASK_POS = 4;
	const uint32_t CUT_ADDRESS_MASK = 0x1F << MASK_POS;
	const uint32_t CUT_ADDRESS_MASK_4X = 0x7F << MASK_POS;
    const uint32_t CUT_ADDRESS_MASK_10b = (1 << 11) - 1;
	const uint32_t CUTS = 32;
	
	std::vector<uint32_t> histogram(CUTS);
	std::vector<uint32_t> histogram_prop(CUTS);
	uint32_t addr = 0;
	uint32_t addr_7b = 0;
    uint32_t addr_10b = 0;
	uint32_t addr_offset = 0;
    uint32_t addr_stagger = 0;

	std::vector<uint32_t> cut_assignment(MAX_ADDRESS >> 4);
	std::vector<uint32_t> cut_assignment_prop(MAX_ADDRESS >> 4);
	std::ofstream cut_to_address_map("ram_cut_to_address_map.csv");
	cut_to_address_map << "Address,RAM cut addr.,Start offset,RAM cut (VPU2), RAM cut (VPU3), Stagger bits,";

    for (uint32_t b = 0; b < 6; ++b)
    {        
        cut_to_address_map << "start_bits(" << b << ")";

        if (b != 5)
            cut_to_address_map << ",";
    }
    
    cut_to_address_map << std::endl;

	for (uint32_t a = 0; a < MAX_ADDRESS; a+=16)
	{
        addr_10b = (a >> MASK_POS) & CUT_ADDRESS_MASK_10b;
        addr_stagger = addr_10b >> 5;
		// old mapping
		addr = (a & CUT_ADDRESS_MASK) >> MASK_POS;
		histogram[addr]++;
		cut_assignment[a >> 4] = addr;
		// new mapping
		addr = (a & CUT_ADDRESS_MASK_4X) >> MASK_POS;
		addr_7b = addr;
		addr_offset = (addr >> 5) << 3;
		addr += addr_offset;
		addr = addr & 0x1F;
		histogram_prop[addr]++;
		cut_assignment_prop[a >> 4] = addr;

        cut_to_address_map
            << a << ","
            << addr_7b << ","
            << addr_offset << ","
            << std::dec
            << cut_assignment[a >> 4] << ","
            << cut_assignment_prop[a >> 4] << ","
            << addr_stagger << ",";

        for (uint32_t b = 0; b < 6; ++b)
        {
            uint32_t addr_b = (1 << b) - 1;
            addr_b &= addr_stagger;
            uint32_t shift_r = 9 - b;
            uint32_t phy_addr = a & ~0x1ff;
            phy_addr += (512  - (addr_b << shift_r) + (a & 0x1ff)) % 512;
            cut_to_address_map  << phy_addr;

            if (b != 5)
                cut_to_address_map << ",";
        }
			
        cut_to_address_map << std::endl;
    }

	cut_to_address_map.close();
}

