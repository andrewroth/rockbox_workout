class ExerciseSet < ActiveRecord::Base
  include Csv

  belongs_to :exercise

  def name_with_exercise
    "#{exercise.name} - #{name} [#{exercise.exercise_type.variables.join(', ')}]"
  end
end
