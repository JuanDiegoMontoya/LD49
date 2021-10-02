#include "components.h"

glm::mat4 Transform::GetModel() const
{
  return glm::translate(glm::mat4(1.0), position)
    * glm::mat4_cast(rotation)
    * glm::scale(glm::mat4(1.0), scale);
}
