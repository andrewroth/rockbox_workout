class WorkoutDate < ActiveRecord::Base
  include Csv

  belongs_to :workout
  has_many :exercise_log_entries

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL")
    end
  end

  def self.read_csv
    f = File.read(Rails.root.join("transfer/workout_dates.csv"))
    wd = nil
    f.split("\n").each do |line|
      name, type, value = line.split(",")
      case name
      when 'id'
        # save the previous workout date
        if wd
          wd.save!
        end

        wd = WorkoutDate.find value
        #wd = WorkoutDate.find_or_create_by_id value
      when 'started_at_int'
        wd.started_at = Time.at(value.to_i) if value != '0'
      when 'created_at_int'
        wd.created_at = Time.at(value.to_i) if value != '0'
      end
    end
    wd.save! if wd # save the last workout date
  end
end
