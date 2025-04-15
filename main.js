const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql');
const cors = require('cors');
const app = express();
const port = process.env.PORT || 5000;

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.urlencoded({ extended: true }));
app.use(express.json());
app.use(cors());

const pool = mysql.createPool({
    connectionLimit: 10,
    host: 'localhost',
    user: 'root',
    password: '',
    database: 'db_project'
});

app.listen(port, () => console.log(`Listening on port ${port}`));
// //Start Operation
// app.post('/register', (req, res) => {
//     console.log('API called: /register');

//     pool.getConnection((err, connection) => {
//         if (err) {
//             console.error('Error connecting to the database:', err);
//             return res.status(500).send('Database connection error.');
//         }

//         console.log('Database connection established');

//         const params = req.body;
//         console.log('Request Body:', params);

        

//         connection.query('INSERT INTO member SET ?', params, (err, rows) => {
//             connection.release();

//             if (!err) {
//                 console.log('Insert Successful:', rows);
//                 res.status(201).send(`Member with ID ${rows.insertId} has been added.`);
//             } else {
//                 console.error('Error inserting data:', err);
//                 res.status(500).send('Failed to add member.');
//             }
//         });
//     });
// });
app.post('/register', (req, res) => {
    console.log('API called: /register');

    pool.getConnection((err, connection) => {
        if (err) {
            console.error('Error connecting to the database:', err);
            return res.status(500).send({ message: 'Database connection error.' });
        }

        console.log('Database connection established');

        const params = req.body;
        console.log('Request Body:', params);

        connection.query('INSERT INTO member SET ?', params, (err, rows) => {
            connection.release();

            if (!err) {
                console.log('Insert Successful:', rows);
                // Sending JSON response instead of plain text
                res.status(201).json({
                    member_Id: rows.insertId,  // Assuming this is the member's ID
                    package: params.membership  // Assuming the package is sent from the front-end
                });
            } else {
                console.error('Error inserting data:', err);
                res.status(500).send({ message: 'Failed to add member.' });
            }
        });
    });
});
//Get all data
app.get('/member', (req, res) => {
    pool.getConnection((err, connection) => {
        if (err) {
            console.error('Error connecting to the database:', err);
            res.status(500).send('Database connection error.');
            return;
        }

        connection.query('SELECT * FROM member', (err, rows) => {
            connection.release(); // Return the connection to the pool
            if (!err) {
                res.status(200).json(rows); // Return all rows as JSON
            } else {
                console.error('Error executing query:', err);
                res.status(500).send('Failed to retrieve data.');
            }
        });
    });
});


//get data by id
app.get('/member/:id', (req, res) => {
    pool.getConnection((err, connection) => {
        if (err) {
            console.error('Error connecting to the database:', err);
            res.status(500).send('Database connection error.');
            return;
        }

        const member_Id = req.params.id; // Retrieve ID from request parameters
        connection.query('SELECT * FROM member WHERE member_id = ?', [member_Id], (err, rows) => {
            connection.release(); // Return the connection to the pool
            if (!err) {
                if (rows.length > 0) {
                    res.status(200).json(rows[00]); // Return the specific member as JSON
                } else {
                    res.status(404).send('Member not found.'); // Handle case where no member is found
                }
            } else {
                console.error('Error executing query:', err);
                res.status(500).send('Failed to retrieve data.');
            }
        });
    });
});
//Update data
app.put('/member/:id', (req, res) => {
    pool.getConnection((err, connection) => {
        if (err) {
            console.error('Error connecting to the database:', err);
            res.status(500).send('Database connection error.');
            return;
        }

        const memberId = req.params.id; // Retrieve the ID from the URL
        const { FULL_NAME, EMAIL, PHONE, PASSWORD, ADDRESS, MEMBERSHIP } = req.body; // Data to update

        if (!FULL_NAME || !EMAIL || !PHONE || !PASSWORD || !ADDRESS || !MEMBERSHIP) {
            res.status(400).send('All fields are required.');
            return;
        }

        const updateQuery = `
            UPDATE member 
            SET FULL_NAME = ?, EMAIL = ?, PHONE = ?, PASSWORD = ?, ADDRESS = ?, MEMBERSHIP = ? 
            WHERE member_id = ?
        `;

        connection.query(updateQuery, [FULL_NAME, EMAIL, PHONE, PASSWORD, ADDRESS, MEMBERSHIP, memberId], (err, results) => {
            connection.release(); // Return the connection to the pool
            if (!err) {
                if (results.affectedRows > 0) {
                    res.status(200).send(`Member with ID ${memberId} has been updated.`);
                } else {
                    res.status(404).send('Member not found.');
                }
            } else {
                console.error('Error executing query:', err);
                res.status(500).send('Failed to update member.');
            }
        });
    });
});
//Delete member data by id
app.delete('/member/:id', (req, res) => {
    pool.getConnection((err, connection) => {
        if (err) {
            console.error('Error connecting to the database:', err);
            res.status(500).send('Database connection error.');
            return;
        }

        const memberId = req.params.id; // Retrieve the ID from the URL

        const deleteQuery = 'DELETE FROM member WHERE member_id = ?';

        connection.query(deleteQuery, [memberId], (err, results) => {
            connection.release(); // Return the connection to the pool
            if (!err) {
                if (results.affectedRows > 0) {
                    res.status(200).send(`Member with ID ${memberId} has been deleted.`);
                } else {
                    res.status(404).send('Member not found.');
                }
            } else {
                console.error('Error executing query:', err);
                res.status(500).send('Failed to delete member.');
            }
        });
    });
});
//Get Operation for trainer by id
app.get('/trainer/:id', (req, res) => {
    pool.getConnection((err, connection) => {
        if (err) {
            console.error('Error connecting to the database:', err);
            res.status(500).send('Database connection error.');
            return;
        }

        const trainerId = req.params.id; // Retrieve ID from request parameters
        connection.query('SELECT name, experience, expertise, branch FROM trainer WHERE id = ?', [trainerId], (err, rows) => {
            connection.release(); // Return the connection to the pool
            if (!err) {
                if (rows.length > 0) {
                    res.status(200).json(rows[0]); // Return the specific trainer as JSON
                } else {
                    res.status(404).send('Trainer not found.');
                }
            } else {
                console.error('Error executing query:', err);
                res.status(500).send('Failed to retrieve data.');
            }
        });
    });
});
//Insert Operation for Feedback by id
app.post('/feedback', (req, res) => {
    console.log('API called: /feedback');

    pool.getConnection((err, connection) => {
        if (err) {
            console.error('Error connecting to the database:', err);
            return res.status(500).send('Database connection error.');
        }

        console.log('Database connection established');

        const params = req.body;
        console.log('Request Body:', params);

        connection.query('INSERT INTO feedback SET ?', params, (err, rows) => {
            connection.release();

            if (!err) {
                console.log('Insert Successful:', rows);
                res.status(201).send(`Feedback with ID ${rows.insertId} has been added.`);
            } else {
                console.error('Error inserting data:', err);
                res.status(500).send('Failed to add feedback.');
            }
        });
    });
});
// //members with trainers
// app.get('/members-with-trainers', (req, res) => {
//     const sqlQuery = `
//         SELECT 
//             m.MEMBER_ID, 
//             m.FULL_NAME, 
//             m.EMAIL, 
//             m.PHONE, 
//             m.ADDRESS, 
//             m.MEMBERSHIP, 
//             t.name AS Trainer_Name, 
//             t.experience AS Trainer_Experience, 
//             t.expertise AS Trainer_Expertise, 
//             t.branch AS Trainer_Branch
//         FROM 
//             member m
//         JOIN 
//             trainer t 
//         ON 
//             m.TRAINER_ID = t.id;
//     `;

//     pool.query(sqlQuery, (err, results) => {
//         if (err) {
//             console.error('Error fetching data:', err);
//             res.status(500).send('Internal server error');
//         } else {
//             res.status(200).json(results);
//         }
//     });
// });

// app.post('/feedback', (req, res) => {
//     console.log('API called: /feedback');

//     pool.getConnection((err, connection) => {
//         if (err) {
//             console.error('Error connecting to the database:', err);
//             return res.status(500).send('Database connection error.');
//         }

//         console.log('Database connection established');

//         const { FULL_NAME, EMAIL, MEMBER_ID, FEEDBACK_TEXT } = req.body;

//         // Validate input
//         if (!FULL_NAME || !EMAIL || !MEMBER_ID || !FEEDBACK_TEXT) {
//             connection.release();
//             return res.status(400).send('All fields are required.');
//         }

//         // Check if MEMBER_ID exists in the member table
//         connection.query('SELECT * FROM member WHERE MEMBER_ID = ?', [MEMBER_ID], (err, results) => {
//             if (err) {
//                 connection.release();
//                 console.error('Error checking MEMBER_ID:', err);
//                 return res.status(500).send('Internal server error.');
//             }

//             if (results.length === 0) {
//                 connection.release();
//                 return res.status(400).send('Invalid MEMBER_ID. The member does not exist.');
//             }

//             // Insert feedback into the feedback table
//             connection.query(
//                 'INSERT INTO feedback (FULL_NAME, EMAIL, MEMBER_ID, FEEDBACK_TEXT) VALUES (?, ?, ?, ?)',
//                 [FULL_NAME, EMAIL, MEMBER_ID, FEEDBACK_TEXT],
//                 (err, rows) => {
//                     connection.release();

//                     if (err) {
//                         console.error('Error inserting data:', err);
//                         return res.status(500).send('Failed to add feedback.');
//                     }

//                     console.log('Insert Successful:', rows);
//                     res.status(201).send(`Feedback added successfully with ID ${rows.insertId}.`);
//                 }
//             );
//         });
//     });
// });
// //Transaction
// connection.beginTransaction((err) => {
//     if (err) throw err;

//     // Insert the new member
//     connection.query(
//         'INSERT INTO members (full_name, email, phone, membership, trainer_id) VALUES (?, ?, ?, ?, ?)',
//         [fullName, email, phone, membership, trainerId],
//         (err, results) => {
//             if (err) {
//                 return connection.rollback(() => {
//                     throw err;
//                 });
//             }

//             // Update trainer's member count
//             connection.query(
//                 'UPDATE trainers SET assigned_members = assigned_members + 1 WHERE trainer_id = ?',
//                 [trainerId],
//                 (err, results) => {
//                     if (err) {
//                         return connection.rollback(() => {
//                             throw err;
//                         });
//                     }

//                     // Commit transaction
//                     connection.commit((err) => {
//                         if (err) {
//                             return connection.rollback(() => {
//                                 throw err;
//                             });
//                         }
//                         console.log('Transaction Complete.');
//                     });
//                 }
//             );
//         }
//     );
// });
