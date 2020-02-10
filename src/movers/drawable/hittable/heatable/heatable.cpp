#include "heatable.h"

heatable::heatable(const std::string path) :
  hittable(path),
  heat(0),
  max_heat(xmlparse::get().get_xml_float(
    path + "/heat/max_heat"
  )),
  max_overheat(xmlparse::get().get_xml_float(
    path + "/heat/max_overheat"
  )),
  boost_heat_per_tick(xmlparse::get().get_xml_float(
    path + "/heat/boost_heat_per_tick"
  )),
  cool_per_tick(xmlparse::get().get_xml_float(
    path + "/heat/cool_per_tick"
  ))
{ }

void heatable::boost(bool b) {
  return;
}

float heatable::get_heat_percent() {
  float retval = 0;
  retval = (
    max_heat != 0
    ? (
      heat - max_heat < 0
      ? heat / max_heat
      : 1
    )
    : -1
  );
  return retval;
}

float heatable::get_overheat_percent() {
  float retval = 0;
  retval = (
    max_overheat - max_heat != 0
    ? (
      heat - max_heat >= 0
      ? (heat - max_heat) / (max_overheat - max_heat)
      : 0
    )
    : -1
  );
  return retval;
}

void heatable::update() {
  //check to see if we're overheated
  if(heat > max_overheat) { overheated = true; }
  if(heat < max_heat) { overheated = false; }
  if(overheated) { boosted = false; }

  //call this first since boost is switched off
  //in parent update call
  if(boosted) {
    heat += boost_heat_per_tick;
  }

  hittable::update();

  //decay some heat
  cool_dn(cool_per_tick);
}

void heatable::draw() const {
  hittable::draw();
}
