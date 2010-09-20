class Exercise < ActiveRecord::Base
  belongs_to :exercise_type

  def name_with_type
    "#{name} (#{exercise_type.try(:name)})"
  end
end
