/******************************************************************************
\File ocean.cpp
\Author Timothy Lee Ke xin
\Par DP email: timothykexin.lee@digipen.edu.sg
\Par Course: RSE 1202
\Par Section: Battleship 
\Par Programming Assignment #2
\Date 27-01-2022

  \Brief
  This program is used for the implementation of a simple version of the board 
  game Battleship. There are 5 functions in total to simulate the game. The 
  rules of the game are as follows:
  1) Each ship/boat occupies BOAT_LENGTH (4) spaces and are set horizontally or 
     vertically. 
  2) Boats are to be placed within the ocean and the initial tiles of the ocean 
     is set to DamageType::dtOK (0), if the boat can be placed into the ocean, 
     the ID of that boat replaces the 0. 
  3) When a shot is taken, the will be an output to display if the shot was 
     ILLEGAL (outisde of ocean), HIT (a boat was hit), MISS (empty tile hit),
     DUPLICATE (tile was shot at previously), SUNK (all 4 tiles of the boat 
     was hit).
  4) If the shot hit an empty tile, the tile will be set to 
     DamageType::dtBLOWNUP (-1). If it hit a boat the tile will be set to the 
     boat's ID + HIT_OFFSET (100). 
  
  - CreateOcean
      Creates the dynamically allocated ocean of int with horizontal by 
      vertical size as denoted by y and x axis, and assigned with its 
      appropriate values (mainly 0) based on the struct Ocean and what 
      encompasses it. It sets the ocean's grid, num_boats, boats hits and ID, 
      and ShotStats stats variables to contain zeros. x_size and y_size to 
      contain the corresponding values. Boat boats is also dynamically 
      allocated with size num_boats. It then returns the struct ocean pointer.

  - DestroyOcean
      Deallocates the dynamically allocated memory in the function CreateOcean.
      Returns nothing.
      
  - PlaceBoat
      Places the boats into the grid of the ocean. The boat must be of 
      BOAT_LENGTH size and fit into the ocean's grid. It must not overlap with
      other boats and should it be placed, it shall be done so in the specified
      orientation, horizontally or vertically. Once placed, the tiles will have 
      the boat's ID. It then returns a BoatPlacement value to indicate if the 
      specified placement is accepted or rejected.

  - Takeshot
      Takes the grid locations to indicate the target. If an empty tile is hit
      the tile's value will be -1 and returns MISS. If a location outside the 
      grid was specified, this function returns ILLEGAL. If the tile containing
      a boat was hit, it alters the tile to contain the boat ID + 100, and 
      returns HIT. If 4 tiles of the boat has been hit, it returns SUNK. If the 
      grid location was indicated again, it returns DUPLICATE.

  - GetShotStats
      Returns the statistics from the free store object referenced by ocean of
      type ShotStats stats.
 
*******************************************************************************/
#include "ocean.h"
#include <iostream> // std::cout
#include <iomanip>  // std::setw

namespace HLP2 {
  namespace WarBoats {
    int const BOAT_LENGTH {4};   //!< Length of a boat
    int const HIT_OFFSET  {100}; //!< Add this to the boat ID

/******************************************************************************/
/*! 
  \brief
    Creates the dynamically allocated ocean of int with horizontal by vertical 
    size as denoted by y and x axis, and assigned with its appropriate values 
    (mainly 0) based on the struct Ocean and what encompasses it. It sets the 
    ocean's grid, num_boats, boats hits and ID, and ShotStats stats variables 
    to contain zeros. x_size and y_size to contain the corresponding values. 
    Boat boats is also dynamically allocated with size num_boats. It then 
    returns the struct ocean pointer.

  \param num_boats
    Contains the number of boats.

  \param x_size
    The horizontal number of columns the grid should contain.

  \param y_size
    The vertical number of rows the grid should contain.

  \param ocean
    The struct Ocean pointer to be dynamically allocated.

  \param i
    Indicator for number of rows in a for loop.

  \param j
    Indicator for number of columns in a for loop.
*/    
/*******************************************************************************/
    Ocean* CreateOcean(int num_boats, int x_size, int y_size) {
      struct Ocean *ocean = {new Ocean [y_size * x_size]};    //dynamically allocate one large block of memory for 2D array[y][x]
      ocean->grid = {new int [y_size * x_size]};
      for (int i = 0; i < y_size; ++i) {
        for (int j {}; j < x_size; ++j) {
          *(ocean->grid + i * x_size + j) = DamageType::dtOK; //set 0 as initial value or grid[][]
        }
      }
      ocean->boats = {new Boat [num_boats]};
      ocean->boats->hits = DamageType::dtOK;
      ocean->boats->ID = 0; 
      ocean->x_size = x_size;
      ocean->y_size = y_size;
      ocean->stats = {0, 0, 0, 0};

      return ocean;
    }

/******************************************************************************/
/*! 
  \brief
    Deallocates the dynamically allocated memory in the function CreateOcean.
    Returns nothing.

  \param theOcean
    The struct Ocean pointer.
*/
/*******************************************************************************/
    void DestroyOcean(Ocean *theOcean) {
      delete theOcean->boats; //deallocate boats memory
      //for (int i = 0; i < (theOcean->y_size); i++)
      //  delete[] (theOcean->grid+i); //deallocate grid inner array memory
      delete[] theOcean->grid;  //deallocate grid outer array memory
      delete[] theOcean;  //deallocate struct Ocean memory
    }

/******************************************************************************/
/*! 
  \brief
    Places the boats into the grid of the ocean. The boat must be of 
    BOAT_LENGTH size and fit into the ocean's grid. It must not overlap with
    other boats and should it be placed, it shall be done so in the specified
    orientation, horizontally or vertically. Once placed, the tiles will have 
    the boat's ID. It then returns a BoatPlacement value to indicate if the 
    specified placement is accepted or rejected.

  \param ocean
    The struct Ocean pointer.

  \param boat
    The struct Boat pointer containing the boat details.

  \param xcean
    Contains the x axis size of the ocean.

  \param ycean
    Contains the y axis size of the ocean.

  \param col
    Contains the x axis size of the boat.

  \param row
    Contains the y axis size of the boat.    

  \param i
    Indicator for number of rows/colums in a for loop.
*/
/*******************************************************************************/
    BoatPlacement PlaceBoat(Ocean& ocean, Boat const& boat) {
      int col = boat.position.x, xcean = ocean.x_size;
      int row = boat.position.y, ycean = ocean.y_size;

      if (boat.orientation == Orientation::oHORIZONTAL) { 
        if (row <= ycean) { //if can exist on y axis
          if (col + BOAT_LENGTH > xcean) { //if cannot exit on x axis
            return BoatPlacement::bpREJECTED;
          }
          else {  //can exit on x axis
            for (int i{}; i < BOAT_LENGTH; i++) {
              if ( *(ocean.grid + row * xcean + col+i) > 0) {  //boat exists
                return BoatPlacement::bpREJECTED;                
              }
            }
            for (int i{}; i < BOAT_LENGTH; i++) { //to place boat ID
              *(ocean.grid + row * xcean + col+i) = boat.ID;
            }
            return BoatPlacement::bpACCEPTED;
          }
        }
        else { //cannot fit into y axis
          return BoatPlacement::bpREJECTED;
        }
      }
      else { //if (boat.orientation == Orientation::oVERTICAL)  
        if (col <= xcean) { //check if can exist on x axis
          if (row + BOAT_LENGTH > ycean) { //if cannot exit on y axis
              return BoatPlacement::bpREJECTED;
            }
            else {  //can exit on y axis
              for (int i{}; i < BOAT_LENGTH; i++) {
                if ( *(ocean.grid + (row+i) * xcean + col) > 0) {  //boat exists
                  return BoatPlacement::bpREJECTED;                
                }
              }
              for (int i{}; i < BOAT_LENGTH; i++) { //to place boat ID
                *(ocean.grid + (row+i) * xcean + col) = boat.ID;
              }
              return BoatPlacement::bpACCEPTED;
            }
          }
          else {
            return BoatPlacement::bpREJECTED;
          }
      } 
    }

/******************************************************************************/
/*! 
  \brief
    Takes the grid locations to indicate the target. If an empty tile is hit
    the tile's value will be -1 and returns MISS. If a location outside the 
    grid was specified, this function returns ILLEGAL. If the tile containing a
    boat was hit, it alters the tile to contain the boat ID + 100, and returns
    HIT. If 4 tiles of the boat has been hit, it returns SUNK. If the grid
    location was indicated again, it returns DUPLICATE.

  \param ocean
    The struct Ocean pointer.

  \param coordinate
    The struct Point pointer containing the coordinates of where the shot will
    be taken.

  \param col
    Contains the x coordinate to target.

  \param row
    Contains the y coordinate to target.  

  \param tile
    Pointer to the struct Ocean ocean targeted location on the grid.
*/
/*******************************************************************************/
    ShotResult TakeShot(Ocean& ocean, Point const& coordinate) {
      int col = coordinate.x;
      int row = coordinate.y;
      int *tile = (ocean.grid + row * (ocean.x_size) + col);

      if (col > ocean.x_size || row > ocean.y_size) {
        return ShotResult::srILLEGAL;   //ILLEGAL
      }
      else if (*tile == DamageType::dtOK) {
        *tile = DamageType::dtBLOWNUP; //set tile to -1
        (ocean.stats.misses)++; //increment misses count
        return ShotResult::srMISS;
      }
      else if (*tile == DamageType::dtBLOWNUP 
      || *tile > HIT_OFFSET) {
        (ocean.stats.duplicates)++; //increment duplicates count
        return ShotResult::srDUPLICATE;
      }
        (ocean.stats.hits)++;  //increment hits count
        ((ocean.boats+*tile)->hits)++; //boat id hit counter

        if (((ocean.boats+*tile)->hits) == 4) { 
        *tile += HIT_OFFSET;  //increment tile by hit_offset
        (ocean.stats.sunk)++;
        return ShotResult::srSUNK;
        }

      *tile += HIT_OFFSET;  //increment tile by hit_offset
      return ShotResult::srHIT; //Hit as last return cuz if it falls into any other
                                //condition, it returns something else 
    }

/******************************************************************************/
/*! 
  \brief
    Returns the statistics from the free store object referenced by ocean of
    type ShotStats stats.

  \param theOcean
    The struct Ocean pointer.
*/
/*******************************************************************************/        
    ShotStats GetShotStats(Ocean const& ocean) {
      return ocean.stats;
    }

    /**************************************************************************/
    /*!
      \brief
        Prints the grid (ocean) to the screen.
      
      \param ocean
        The Ocean to print
      
      \param field_width
        How much space each position takes when printed.
      
      \param extraline
        If true, an extra line is printed after each row. If false, no extra
        line is printed.
        
      \param showboats
        If true, the boats are shown in the output. (Debugging feature)
    */
    /**************************************************************************/
    void DumpOcean(const HLP2::WarBoats::Ocean &ocean,
                                    int field_width, 
                                    bool extraline, 
                                    bool showboats) {
      for (int y = 0; y < ocean.y_size; y++) { // For each row
        for (int x = 0; x < ocean.x_size; x++) { // For each column
            // Get value at x/y position
          int value = ocean.grid[y * ocean.x_size + x];
            // Is it a boat that we need to keep hidden?
          value = ( (value > 0) && (value < HIT_OFFSET) && (showboats == false) ) ? 0 : value;
          std::cout << std::setw(field_width) << value;
        }
        std::cout << "\n";
        if (extraline) { std::cout << "\n"; }
      }
    }
  } // namespace WarBoats
} // namespace HLP2
