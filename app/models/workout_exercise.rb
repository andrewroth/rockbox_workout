class WorkoutExercise < ActiveRecord::Base
  include Csv

  belongs_to :workout
  belongs_to :exercise

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL").collect(&:workout).flatten.collect(&:workout_exercises).flatten.uniq
    end
  end
end
