#include "chunk.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
#include "src/image/image.h"
#include "src/image/image_handler.h"
#include "src/xml_parser/xmlparse.h"
#include "src/entity_handler/entity_handler.h"
#include "src/utils/rng.h"

//TODO: remove when done with testing
#include "src/utils/message.h"
#include "src/text/text.h"

const unsigned char chunk::IN = 0;
const unsigned char chunk::UP = 1;
const unsigned char chunk::DN = 2;
const unsigned char chunk::LF = 4;
const unsigned char chunk::RT = 8;
const float chunk::length = 500;

const uint8_t chunk::INITIAL = 0;
const uint8_t chunk::CLOSET = 1;

chunk::chunk(vec2d &v) :
  tlc(v),
  border {0, 0, 0, 0},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
  { }

chunk::chunk(float x, float y) :
  tlc(x*length, y*length),
  border {0, 0, 0, 0},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
  { }

chunk::chunk(vec2d &v, bool u, bool d, bool l, bool r) :
  tlc(v),
  border {u, d, l, r},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
  { }

chunk::chunk(float x, float y, bool u, bool d, bool l, bool r) :
  tlc(x*length, y*length),
  border {u, d, l, r},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
  { }

chunk::chunk(float x, float y, bool u, bool d, bool l, bool r, std::string type) :
  tlc(x*length, y*length),
  border {u, d, l, r},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
{
  if(type != "") {
    //load the chunk data here
    in_bounds = xmlparse::get().get_xml_bool("/chunk_types/"+type+"/in_bounds");
  }
  else {
    in_bounds = true;
  }
}

chunk::chunk(const chunk&c) :
  tlc(c.tlc),
  i1_name(c.i1_name),
  i2_name(c.i2_name),
  frame_bump(c.frame_bump),
  in_bounds(c.in_bounds)
{
  this->set_b_up(c.is_b_up());
  this->set_b_dn(c.is_b_dn());
  this->set_b_lf(c.is_b_lf());
  this->set_b_rt(c.is_b_rt());
}

chunk &chunk::operator=(const chunk& c) {
  //check if these are the same chunk
  if(this == &c) { return *this; }

  this->set_b_up(c.is_b_up());
  this->set_b_dn(c.is_b_dn());
  this->set_b_lf(c.is_b_lf());
  this->set_b_rt(c.is_b_rt());

  this->tlc = c.tlc;

  this->i1_name = c.i1_name;
  this->i2_name = c.i2_name;
  this->frame_bump = c.frame_bump;

  this->in_bounds = c.in_bounds;

  return *this;
}

chunk::~chunk() {
  for(auto r : spawn_rules) {
    delete r;
  }
  spawn_rules.clear();
}

unsigned char chunk::chunk_pos(vec2d &v) const {
  return( this->chunk_pos(v[0], v[1]) );
}

unsigned char chunk::chunk_pos(float x_coord, float y_coord) const {
  unsigned char retval = 0;
  //return 0 if it's in this chunk - otherwise, use bitwise
  //OR to add the location qualifiers

  if(x_coord <= tlc[0]) {
    //it's to the left
    retval = retval|LF;
  }
  if(x_coord >= tlc[0] + length) {
    //to the right
    retval = retval|RT;
  }
  if(y_coord <= tlc[1]) {
    //above
    retval = retval|UP;
  }
  if(y_coord >= tlc[1] + length) {
    //below
    retval = retval|DN;
  }

  return retval;
}

unsigned char chunk::chunk_rebuff(vec2d &pos) const {
  unsigned char c = chunk_pos(pos);
  unsigned char ret = 0;
  if(!c) {
    //still in chunk, no check
    return ret;
  }

  //check each "hit" to see if it's also a barrier - if so,
  //rebuff it in that direction
  if((c & UP) && is_b_up()) { ret = ret|UP; }
  if((c & DN) && is_b_dn()) { ret = ret|DN; }
  if((c & LF) && is_b_lf()) { ret = ret|LF; }
  if((c & RT) && is_b_rt()) { ret = ret|RT; }


  return ret;
}

unsigned char chunk::chunk_rebuff_forced(vec2d &pos) const {
  //force a rebuff - this is used to handle issues where it would otherwise
  //be possible to fly into out of bounds chunks

  unsigned char c = chunk_pos(pos);
  unsigned char ret = 0;
  if(!c) {
    //still in chunk, no check
    return ret;
  }

  //check each "hit" to see if it's out of bounds - ignore barriers or not
  //rebuff it in that direction
  if(c & UP) { ret = ret|UP; }
  if(c & DN) { ret = ret|DN; }
  if(c & LF) { ret = ret|LF; }
  if(c & RT) { ret = ret|RT; }

  return ret;
}

void chunk::add_gate(std::string dest, std::string name) {
  has_gate = true;
  g_dest = dest;
  g_name = "/" + name;
}

void chunk::remove_gate() {
  has_gate = false;
  g_dest = "";
  g_name = "";
}

//push back a new spawn rule
void chunk::add_spawn_rule (
  uint8_t spawn_type, int max_count, int total_count, int tick_spawn_delay,
  const std::string &entity, uint8_t team, float min_spawn_distance,
  float max_spawn_distance, float x_coord, float y_coord, float x_dir_comp,
  float y_dir_comp, float vel_frac
) {
  spawn_rules.push_back(new spawn_rule {
    spawn_type,
    max_count,
    total_count,
    //no entities spawned yet
    0,
    tick_spawn_delay,
    //set initial "spawn checkpoint" to map load time
    t_frame::get().get_t(),
    entity,
    xmlparse::get().get_xml_string("/movers/mortal/" + entity + "/id"),
    team,
    min_spawn_distance,
    max_spawn_distance,
    x_coord,
    y_coord,
    x_dir_comp,
    y_dir_comp,
    vel_frac
  });
}

void chunk::spawn_entities(uint8_t spawn_type) {
  //iterate through spawn rules and look for any marked as "initial"
  for(spawn_rule *r : spawn_rules) {
    //check to see if this matches the desired type
    if(r->spawn_type == spawn_type) {
      //make sure we haven't hit the max count for this entity already
      //(ignored for entities that spawn on arrival)
      if(r->spawn_type == chunk::INITIAL || 
          e_handler::get().get_entity_count_by_id(r->id) < r->max_count) {

        //if it's a closet, process spawn delay here
        if(spawn_type == chunk::CLOSET) {
          if(t_frame::get().get_t() - r->spawn_checkpoint < r->tick_spawn_delay) {
            //too soon to spawn this rule - skip it and go to the next rule
            continue;
          }
        }

        //make sure we haven't burned out the max allowable spawns
        if(r->total_count != -1 && r->spawned_count >= r->total_count) { continue; }


        //make sure player's at an appropriate distance to spawn entity. if
        //player is too close, entity fails to spawn
        if((this->get_midpoint().dist(
            e_handler::get().get_plr_pos()) > r->min_spawn_distance &&
            (this->get_midpoint().dist(e_handler::get().get_plr_pos()) 
                < r->max_spawn_distance || 
            r->max_spawn_distance == -1)
        ) || r->spawn_type == chunk::INITIAL) {
          //at this point, all criteria have been met. set the entity's initial
          //values and request its addition to the game

          //calculate initial position
          vec2d spawn_pos = tlc;
          //the +1 at the end is to prevent entities from spawning directly on borders
          spawn_pos[0] += (r->x_coord != -1 ? r->x_coord : rng::get().get_map(1, length-1));
          spawn_pos[1] += (r->y_coord != -1 ? r->y_coord : rng::get().get_map(1, length-1));


          //calculate initial direction
          //first get the components and normalize them
          vec2d spawn_dir = {r->x_dir_comp, r->y_dir_comp};
          spawn_dir = spawn_dir.normalize();

          //now multiply any components by 100
          spawn_dir = spawn_dir * 100;
          //if any of the components are 0, give them a random value
          if(spawn_dir[0] == 0) { spawn_dir[0] += rng::get().get_map(-100, 100); }
          if(spawn_dir[1] == 0) { spawn_dir[1] += rng::get().get_map(-100, 100); }

          //one last normalize
          spawn_dir = spawn_dir.normalize();

          //finally, check the velocity fraction
          float vel_f = r->vel_frac;
          if(vel_f == -1) {
            //get a random velocity
            vel_f = rng::get().get_map(-100, 100);
            vel_f /= 100;
          }

          //now ask the entity handler to add this entity

          //TODO: add team
          e_handler::get().add_npe("mortal/" + r->entity, spawn_pos, spawn_dir, vel_f);

          //register this as a successful spawn
          r->spawned_count += 1;
          r->spawn_checkpoint = t_frame::get().get_t();
        }
      }
      else {
        r->spawn_checkpoint = t_frame::get().get_t();
      }
    }
  }
  return;
}

void chunk::spawn_initial_entities() {
  spawn_entities(chunk::INITIAL);
}

void chunk::spawn_closet_entities() {
  spawn_entities(chunk::CLOSET);
}

void chunk::draw() const {
  float x = tlc[0];
  float y = tlc[1];
  //draw 2 barriers on each line where border[i] = true -
  //that's where a border is
  //check in order: u, d, l, r
  if(border[0]) {
    //the top is a border
    image_handler::get().draw_v2(i1_name, x + (length / 4), y, frame_bump, 0);
    image_handler::get().draw_v2(i1_name, x + 3 * (length / 4), y, frame_bump, 0);

    image_handler::get().draw_v2(i2_name, x, y, 0, 0);
    image_handler::get().draw_v2(i2_name, x + (length / 2), y, 0, 0);
  }
  if(border[1]) {
    //the bottom is a border
    image_handler::get().draw_v2(i1_name, x + (length / 4), y + length, frame_bump, 180);
    image_handler::get().draw_v2(i1_name, x + 3 * (length / 4), y + length, frame_bump, 180);

    image_handler::get().draw_v2(i2_name, x + (length / 2), y + length, 0, 0);
    image_handler::get().draw_v2(i2_name, x + length, y + length, 0, 0);
  }
  if(border[2]) {
    //the left is a border
    image_handler::get().draw_v2(i1_name, x, y + (length / 4), frame_bump, 270);
    image_handler::get().draw_v2(i1_name, x, y + 3 * (length / 4), frame_bump, 270);

    image_handler::get().draw_v2(i2_name, x, y + (length / 2), 0, 0);
    image_handler::get().draw_v2(i2_name, x, y + length, 0, 0);
  }
  if(border[3]) {
    //the right is a border
    image_handler::get().draw_v2(i1_name, x + length, y + (length / 4), frame_bump, 90);
    image_handler::get().draw_v2(i1_name, x + length, y + 3 * (length / 4), frame_bump, 90);

    image_handler::get().draw_v2(i2_name, x + length, y, 0, 0);
    image_handler::get().draw_v2(i2_name, x + length, y + (length / 2), 0, 0);
  }

  if(has_gate) {
    image_handler::get().draw_v2(g_name, x + length/2, y + length/2, 0, 0);
  }

  //debug_draw();
}

//warning: for reasons i haven't seen fit to investigate this function is
//apparently extremely detrimental to framerate
void chunk::debug_draw() const {
  int x = tlc[0] - viewport::get().get_tlc_x();
  int y = tlc[1] - viewport::get().get_tlc_y();


  SDL_Rect r;
  r.x = x;
  r.y = y;
  r.w = r.h = length;



  //save color
//  SDL_Color c;
  //SDL_GetRenderDrawColor(render::get().get_r(), &(c.r), &(c.g), &(c.b), &(c.a));

  SDL_SetRenderDrawColor(render::get().get_r(), 200, 200, in_bounds ? 200 : 0, 96);

  //draw rect
  SDL_RenderFillRect(render::get().get_r(), &r);
//  SDL_SetRenderDrawColor(render::get().get_r(), 64, 255, 64, 255);
  //SDL_RenderDrawRect(render::get().get_r(), &r);

  //draw borders
  //for debugging purposes, make border walls red and passable walls green
  SDL_SetRenderDrawColor(render::get().get_r(), 255, 64, 64, 255);
  if(border[0]) {
    SDL_RenderDrawLine(render::get().get_r(), x, y, x+length, y);
  }
  if(border[1]) {
    SDL_RenderDrawLine(render::get().get_r(), x, y+length, x+length, y+length);
  }
  if(border[2]) {
    SDL_RenderDrawLine(render::get().get_r(), x, y, x, y+length);
  }
  if(border[3]) {
    SDL_RenderDrawLine(render::get().get_r(), x+length, y, x+length, y+length);
  }

  //restore color
//  SDL_SetRenderDrawColor(render::get().get_r(), c.r, c.g, c.b, c.a);

  //print inbounds or not
  text t(in_bounds ? "in bounds" : "out of bounds", x + 40, y + 40);
  t.draw();


}
