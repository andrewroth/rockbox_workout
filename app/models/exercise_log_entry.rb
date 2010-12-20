class ExerciseLogEntry < ActiveRecord::Base
  include Csv

  has_many :set_log_entries
  belongs_to :exercise

  def self.write_csv
    super("transfer/exercise_logs.csv") do
      WorkoutDate.where("finished_at IS NULL").collect(&:exercise_log_entries)
    end
  end

  def self.read_csv
    f = File.read(Rails.root.join("transfer/exercise_logs.csv"))
    ele = nil
    f.split("\n").each do |line|
      name, type, value = line.split(",")
      case name
      when 'created_at_int'
        ele.save! if ele
        ele = ExerciseLogEntry.find_or_create_by_created_at(Time.at(value.to_i))
      when 'started_at_int'
        ele.started_at = Time.at(value.to_i) if value != '0'
      when 'last_completed_at_int'
        ele.last_completed_at = Time.at(value.to_i) if value != '0'
      when 'finished_at_int'
        ele.finished_at = Time.at(value.to_i) if value != '0'
      when 'workout_date_id'
        ele.workout_date_id = value
      when 'exercise_id'
        ele.exercise_id = value
      end
      ele.save! if ele
    end
  end

end
