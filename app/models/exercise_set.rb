class ExerciseSet < ActiveRecord::Base
  belongs_to :exercise

  def name_with_exercise
    "#{exercise.name} - #{name} [#{exercise.exercise_type.variables.join(', ')}]"
  end
end
