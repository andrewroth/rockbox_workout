class WorkoutExercise < ActiveRecord::Base
  include Csv

  belongs_to :workout
  belongs_to :exercise

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL").collect(&:exercise_log_entries).flatten.collect(&:set_log_entries).flatten
    end
  end
end
