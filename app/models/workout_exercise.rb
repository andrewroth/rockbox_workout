class WorkoutExercise < ActiveRecord::Base
  belongs_to :workout
  belongs_to :exercise
end
