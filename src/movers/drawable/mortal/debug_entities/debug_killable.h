#ifndef MORTAL_DEBUG_KILLABLE_H_
#define MORTAL_DEBUG_KILLABLE_H_

class d_killable : public mortal {
public:
  d_killable(const std::string);

  void update() override;
  void draw() const override;
};

#endif
