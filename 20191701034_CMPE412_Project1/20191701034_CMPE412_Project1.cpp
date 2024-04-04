/*
ENISE NUR YILMAZ
20191701034
CMPE412-PROJECT 1
*/
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
using namespace std;

const int total_years = 200;
const int min_age_quaestor = 30;
const int min_age_aedile = 36;
const int min_age_praetor = 39;
const int min_age_consul = 42;
const int min_service_aedile = 2;
const int min_service_praetor = 2;
const int min_service_consul = 2;
const int consul_reelection_years = 10;
const int eachyear_quaestores = 20;
const int eachyear_aediles = 10;
const int eachyear_praetors = 8;
const int eachyear_consul = 2;
const int PSI_startpoint = 100;
const int penalty_pos_unfilled = -5;
const int continuous_reelection_asConsul = -10;
const int mean_of_influx_annually = 15;
const int stdDev_of_influx_annually = 5;
const int mean_of_life_expentancy = 55;
const int stdDev_of_life_expectancy = 10;

class Politician {
public:
    Politician(int age) : age(age) {}
    int getAge() const { return age; }
    int age;
};

class romeSimulation {
public:
    romeSimulation() : psi(PSI_startpoint), year(0) {
        politicians.reserve(eachyear_quaestores + eachyear_aediles + eachyear_praetors + eachyear_consul);
        for (int i = 0; i < eachyear_quaestores; i++) {
            politicians.emplace_back(min_age_quaestor);
        }
        for (int i = 0; i < eachyear_aediles; i++) {
            politicians.emplace_back(min_age_aedile);
        }
        for (int i = 0; i < eachyear_praetors; i++) {
            politicians.emplace_back(min_age_praetor);
        }
        for (int i = 0; i < eachyear_consul; i++) {
            politicians.emplace_back(min_age_consul);
        }
    }

    void simulateYear(std::mt19937& gen, std::normal_distribution<>& lifeExpectancyDist) {
        std::shuffle(politicians.begin(), politicians.end(), gen);
        for (auto& politician : politicians) {
            politician = Politician(politician.getAge() + 1);
        }

        politicians.erase(std::remove_if(politicians.begin(), politicians.end(),
            [&lifeExpectancyDist, &gen](const Politician& p) { return p.getAge() > lifeExpectancyDist(gen); }), politicians.end());

        psi += penalty_pos_unfilled * (eachyear_quaestores - politicians_count(min_age_quaestor, min_age_aedile)) +
            penalty_pos_unfilled * (eachyear_aediles - politicians_count(min_age_aedile, min_age_praetor)) +
            penalty_pos_unfilled * (eachyear_praetors - politicians_count(min_age_praetor, min_age_consul)) +
            penalty_pos_unfilled * (eachyear_consul - politicians_count(min_age_consul, mean_of_life_expentancy));

        if (year % consul_reelection_years == 0) {
            psi += continuous_reelection_asConsul * (eachyear_consul - politicians_count(min_age_consul, mean_of_life_expentancy));
        }
    }

    int PSI_finalResult() const
    {
        return psi;
    }

    void run_function() {
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<> influxDist(mean_of_influx_annually, stdDev_of_influx_annually);
        normal_distribution<> lifeExpectancyDist(mean_of_life_expentancy, stdDev_of_life_expectancy);

        int year = 0;
        while (year < total_years) {
            int influx = static_cast<int>(influxDist(gen));
            for (int i = 0; i < influx; ++i) {
                politicians.emplace_back(min_age_quaestor);
            }
            simulateYear(gen, lifeExpectancyDist);
            year++;
        }
    }


    void dist_of_age_calculation() {
        vector<int> quaestorAges;
        vector<int> aedileAges;
        vector<int> praetorAges;
        vector<int> consulAges;

        for (const auto& politician : politicians) {
            int age = politician.getAge();
            if (age >= min_age_quaestor && age < min_age_aedile) {
                quaestorAges.push_back(age);
            }
            else if (age >= min_age_aedile && age < min_age_praetor) {
                aedileAges.push_back(age);
            }
            else if (age >= min_age_praetor && age < min_age_consul) {
                praetorAges.push_back(age);
            }
            else if (age >= min_age_consul) {
                consulAges.push_back(age);
            }
        }

        auto calc_sum = [](const std::vector<int>& ages) -> std::pair<unsigned __int64, double> {
            if (ages.empty()) {
                return std::make_pair(0ULL, 0.0);
            }

            unsigned __int64 totalAges = 0;
            for (int age : ages) {
                totalAges += age;
            }
            double averageAge = static_cast<double>(totalAges) / ages.size();

            return std::make_pair(static_cast<unsigned __int64>(ages.size()), averageAge);
        };

        auto quaest_sum = calc_sum(quaestorAges);
        auto aedile_sum = calc_sum(aedileAges);
        auto praetor_sum = calc_sum(praetorAges);
        auto consul_sum = calc_sum(consulAges);

        cout << "Age Distribution" << std::endl;
        cout << "Quaestor: Count=" << quaest_sum.first << ", Average Age=" << quaest_sum.second << endl;
        cout << "Aedile: Count=" << aedile_sum.first << ", Average Age=" << aedile_sum.second << endl;
        cout << "Praetor: Count=" << praetor_sum.first << ", Average Age=" << praetor_sum.second << endl;
        cout << "Consul: Count=" << consul_sum.first << ", Average Age=" << consul_sum.second << endl;
    }

private:
    int psi;
    int year;
    std::vector<Politician> politicians;

    int politicians_count(int minAge, int maxAge) {
        return std::count_if(politicians.begin(), politicians.end(),
            [minAge, maxAge](const Politician& p) { return p.getAge() >= minAge && p.getAge() < maxAge; });
    }
};

int main() {

    romeSimulation romeSimulation;
    romeSimulation.run_function();

    std::cout << "End-of-romeSimulation PSI: " << romeSimulation.PSI_finalResult() << std::endl;

    std::cout << "Annual Fill Rate:" << std::endl;
    std::cout << "Quaestor In Percentages: " << static_cast<double>(eachyear_quaestores) / total_years * 100 << "%" << std::endl;
    std::cout << "Aedile In Percentages: " << static_cast<double>(eachyear_aediles) / total_years * 100 << "%" << std::endl;
    std::cout << "Praetor In Percentages: " << static_cast<double>(eachyear_praetors) / total_years * 100 << "%" << std::endl;
    std::cout << "Consul In Percentages: " << static_cast<double>(eachyear_consul) / total_years * 100 << "%" << std::endl;

    romeSimulation.dist_of_age_calculation();

    return 0;
}