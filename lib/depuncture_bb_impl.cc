/* -*- c++ -*- */
/* 
 * Copyright 2014 Institute for Theoretical Information Technology,
 *                RWTH Aachen University
 *                www.ti.rwth-aachen.de
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "depuncture_bb_impl.h"

#include <string.h>
#include <algorithm>
#include <iostream>

#define DEBUG 1

namespace gr {
  namespace ofdm {

    depuncture_bb::sptr
    depuncture_bb::make(int vlen,char fillval)
    {
      return gnuradio::get_initial_sptr
        (new depuncture_bb_impl(vlen, fillval));
    }

    /*
     * The private constructor
     */
    depuncture_bb_impl::depuncture_bb_impl(int vlen,char fillval)
      : gr::block("depuncture_bb",
              gr::io_signature::make3(3, 3, sizeof(char),             // bit stream
  	   	   	   	   	   sizeof(char)*vlen,            // modemap
					   sizeof(char) ),
              gr::io_signature::make(1, 1, sizeof(char)))
    	, d_vlen(vlen)
    	, d_need_bits( 0 )
    	, d_out_bits( 0 )
    	, d_fillval(fillval)
    	, d_need_modemap( 1 )
    	, d_modemap(new char[vlen])
    		//d_rep_per_mode( {1,1,1,1,1,1,1,1,1} ) // test case
    	, d_rep_per_mode( {2,4,1,8,2,4,3,2,4} )
    {}

    void depuncture_bb_impl::set_punctpat(char c_mode)
    {
    	unsigned int mode = (unsigned int)c_mode;

    	if(!d_punctpat.empty())
    	{
    		d_punctpat.clear();
    	}

    	if(mode==1||mode==2||mode==4)
    	{
    		d_punctpat.push_back(1);
    	}
    	else if(mode==3||mode==5||mode==7||mode==9)
    	{
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(0);
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(0);
    	}
    	else if(mode==6)
    	{
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(0);
    		d_punctpat.push_back(1);
    	}
    	else if(mode==8)
    	{
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(0);
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(0);
    		d_punctpat.push_back(0);
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(1);
    		d_punctpat.push_back(0);
    	}
    }

    int
    depuncture_bb_impl::calc_bit_amount( const char* modemap, const int& vlen)
    {
      int bits_per_symbol = 0;
      int size_per_mode[9] = {1,1,4,1,4,3,4,6,4};
      //int rep_per_mode[9] = {1,1,1,1,1,1,1,1,1};
      //int rep_per_mode[9] = {2,4,1,8,2,4,3,2,4};

      for(int i = 0; i < vlen; ++i) {
    	if(modemap[i] > 9)
    	  throw std::out_of_range("DEPUNCTURE: Mode higher than 9 not supported");
    	if(modemap[i] < 0)
    	  throw std::out_of_range("DEPUNCTURE: Cannot allocate less than zero bits");
    	if(modemap[i] > 0)
    		bits_per_symbol += size_per_mode[modemap[i]-1]*d_rep_per_mode[modemap[i]-1];
      }

      return bits_per_symbol;
      if(DEBUG)
               std::cout << "BITS_PER_SYMBOL: " << bits_per_symbol << " items";

    }

    int
    depuncture_bb_impl::calc_out_bit_amount( const char* modemap, const int& vlen)
    {
      int out_bits_per_symbol = 0;
      int out_size_per_mode[9] = {1,1,6,1,6,4,6,10,6};
      //int rep_per_mode[9] = {1,1,1,1,1,1,1,1,1};
      //int rep_per_mode[9] = {2,4,1,8,2,4,3,2,4};

      for(int i = 0; i < vlen; ++i) {
    	if(modemap[i] > 9)
    	  throw std::out_of_range("DEPUNCTURE: Mode higher than 9 not supported");
    	if(modemap[i] < 0)
    	  throw std::out_of_range("DEPUNCTURE: Cannot allocate less than zero bits");
    	if(modemap[i] > 0)
    		out_bits_per_symbol += out_size_per_mode[modemap[i]-1]*d_rep_per_mode[modemap[i]-1];
      }

      return out_bits_per_symbol;
      if(DEBUG)
               std::cout << "OUT_BITS_PER_SYMBOL: " << out_bits_per_symbol << " items";

    }
    /*
     * Our virtual destructor.
     */
    depuncture_bb_impl::~depuncture_bb_impl()
    {
    }

    void
    depuncture_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
    	ninput_items_required[0] = d_need_bits;
    	ninput_items_required[1] = d_need_modemap;
    	ninput_items_required[2] = noutput_items;
    }

    int
    depuncture_bb_impl::noutput_forecast( gr_vector_int &ninput_items,
        int available_space, int max_items_avail, std::vector<bool> &input_done )
    {

      if( ninput_items[0] < d_need_bits ){
        if( input_done[0] ){
          return -1;
        }

        return 0;
      }

      if( ninput_items[1] < d_need_modemap ){
        if( input_done[1] ){
          return -1;
        }

        return 0;
      }

      if( ninput_items[2] == 0 && input_done[2] ){
        return -1;
      }

      return std::min( available_space, ninput_items[2] );

    }

    int
    depuncture_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
    		unsigned int j;
    		const char *in = (const char *) input_items[0];
    		const char *modemap = static_cast<const char*>(input_items[1]);
    		const char *trig = static_cast<const char*>(input_items[2]);
    		char *out = (char *) output_items[0];

    		int n_bits = ninput_items[0];
    		int n_modemap = ninput_items[1];
    		int n_trig = ninput_items[2];
    		int nout = noutput_items;

    		int noutsymbols = noutput_items;

    		int n_min = std::min( nout, n_trig );

    		//int n_min = nout;

    		if(DEBUG)
    		std::cout << "[depuncturing " << unique_id() << "] entered, state is "
    				  << "n_bits=" << n_bits << " n_modemap=" << n_modemap
    				  << " n_trig=" << n_trig << " nout=" << nout
    				  << " d_need_bits=" << d_need_bits
    				  << " d_out_bits=" << d_out_bits
    				  << " d_need_modemap=" << d_need_modemap
    				  << " n_min=" << n_min<< std::endl;

    		bool copy = false;
    		const char *map = d_modemap.get();

    		  for( int i = 0; i < n_min; ++i, ++trig ){

    		    if( *trig == 2 )
    		    {

    		      if( n_modemap > 0 )
    		      {

    		        // update modemap buffer
    		        map = modemap;

    		        d_need_bits = calc_bit_amount( map, d_vlen );
    		        d_out_bits = calc_out_bit_amount( map, d_vlen );

    		        // if not enough input, won't consume trigger, therefore
    		        // don't consume modemap item
    		        if( n_bits < d_need_bits ){
    		          d_need_modemap = 1;
    		          break;
    		        }
    		        copy = true;
    		        d_need_modemap = 0;

    		        --n_modemap;
    		        modemap += 2*d_vlen;
    		        consume(1, 1);	// consume modemap item

    		        if(DEBUG)
    		          std::cout << "Consume 1 modemap item, leave " << n_modemap << " items"
    		                    << " and need " << d_need_bits << " bits while "
    		                    << n_bits << " bits left and "
    		                    << nout << " outputs left" << std::endl;
    		      }
    		      else
    		      {
    		        if(DEBUG)
    		          std::cout << "Need modemap flag set" << std::endl;
    		        d_need_modemap = 1;
    		        break;
    		      } // n_modemap > 0
    		    } // *trig == 2
    		    else if( *trig == 1 )
    			{
    				std::cout << "Trigger is 1" << std::endl;
    				if( n_modemap > 0 )
    				{
    					--n_modemap;
    					modemap += 2*d_vlen;
    					consume(1, 1);	// consume modemap item
    					consume(2, 1);

    					d_out_bits = 0;
    					d_need_bits = 0;

    					if(DEBUG)
    					  std::cout << "Consume 1 modemap item (not consumed or used), leave " << n_modemap << " items"
    								<< " and need " << d_need_bits << " bits while "
    								<< n_bits << " bits left and "
    								<< nout << " outputs left" << std::endl;
    				}
    			}

    		    // check if we have enough bits

    		    if( n_bits < d_need_bits ){
    		      if(DEBUG)
    		        std::cout << "Do not have enough bits, need " << d_need_bits
    		                  << " have " << n_bits << std::endl;
    		      break;
    		    } // n_bits < d_need_bits

    		    if( nout < d_out_bits ){
    			  if(DEBUG)
    				std::cout << "Do not have enough output, need " << d_out_bits
    						  << " have " << nout << std::endl;
    			  break;
    			} // nout < d_out_bits

    		    if( d_out_bits == 0 && d_need_bits == 0 && d_need_modemap == 1 ){
    				  if(DEBUG)
    					std::cout << "No usable modemap consumed, yet" << std::endl;
    				  break;
    			} // d_out_bits == 0 && d_need_bits == 0 && d_need_modemap == 1


    		    if(DEBUG)
    		         std::cout << "Puncture OFDM symbol" << std::endl;
    		    // depuncturing block
    		    for( int i = 0; i < d_vlen; ++i )
    		    {
    		      if( map[i] > 0 )
    		      {
    		    	  if(map[i]==1||map[i]==2||map[i]==4)
    		    	  {
    		    		  for( int rep = 0; rep < d_rep_per_mode[map[i]-1]; ++rep )
    		    		  	    		  {
    		    			  	  	  	  //std::cout << "MAP ["<<i<<"]: "<<(float)map[i]<<std::endl;
    									  *out++ = *in++;
    									  if(DEBUG)
    									  					 		std::cout << " . ";
    									  --nout;
    		    		  	    		  }
    		    	  }
    		    	  else
    				  {
    					  set_punctpat(map[i]);
    					  //std::cout << "MAP ["<<i<<"]: "<<map[i]<<std::endl;
    					  for( int rep = 0; rep < d_rep_per_mode[map[i]-1]; ++rep )
    					  {
    					  for (j=0;j<d_punctpat.size();j++)
    					  {
    						if (d_punctpat[j]==1)
    						{
    							*out++ = *in;
    						}
    						else
    						{
    							*out++ = d_fillval;
    						}
    						in++;
    						--nout;
    					  }
    					  }
    				  }
    		      } // map[i] == 0
    		    } // for-loop
    		    n_bits -= d_need_bits;
    		    noutsymbols--;
    		  } // for-loop

    		  // store to internal state variable
    		  if( copy ){
    		    memcpy( d_modemap.get(), map, d_vlen*sizeof(char) );
    		    if(DEBUG)
    		      std::cout << "Copy modemap to internal state buffer" << std::endl;
    		  } // copy

    		  if(DEBUG)
    		    std::cout << "[depuncture] Leaving process, d_need_bits=" << d_need_bits
    		              << " d_need_modemap=" << d_need_modemap << " consume "
    		              << ninput_items[0]-n_bits << " bits and produce "
    		              << noutput_items-nout << " bits\n and "
    		              << noutput_items-noutsymbols << " symbols\n"
    		              << std::endl;

    		  consume(0, std::max(0,ninput_items[0]-n_bits));
    		  consume(2, std::max(0,noutput_items-noutsymbols)); // consume trigger items

    		  return std::max(0,noutput_items-nout);
    }

  } /* namespace ofdm */
} /* namespace gr */

