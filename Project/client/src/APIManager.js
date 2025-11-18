/**
 * Internal function to make POST requests. Use with caution
 * @param {string} url API name
 * @param {JSON} data data to send
 * @param {string} method HTTP method
 * @returns response object
 */
async function post(url = '', data = {}, method = 'POST') {
    try {
        const response = await fetch('http://127.0.0.1:18080' + url, {
            method: method, // Specify HTTP method
            headers: {
                "Content-Type": "application/json", // Tell server we're sending JSON
                "Accept": "application/json"        // Expect JSON response
            },
            body: JSON.stringify(data)
        });

        return response;
    } catch (err) {
        console.error('Failed to fetch API:', err)
        alert("An Internal Error Occurred: " + err.message)
        return null;
    }
}

/**
 * Internal function to make GET requests.  Use with caution
 * @param {string} url API name
 * @returns response object
 */
async function get(url = '') {
    try {
        const response = await fetch('http://127.0.0.1:18080' + url, { method: 'GET' });

        return response;
    } catch (err) {
        console.error('Failed to fetch API:', err)
        alert("An Internal Error Occurred: " + err.message)
        return null;
    }
}

/**
 * Creates a new user in the database. Returns true on success
 * @param {string} username username
 * @param {string} password password (unhashed)
 * @returns boolean indicating success
 */
async function registerUser(username, password) {
    const response = await post('/register', {username: username, password: getStringHashCode(password)});

    if (response === null) return null;

    switch (response.status) {
        case 201:
            alert("User Created Successfully!")
            return true;
        case 409:
            alert("Username Already Exist!")
            return false;
        case 400:
            alert("Username Too Long!")
            return false;
        case 500:
            alert("Internal Server Error")
            return false;
        default:
            alert("An Unexpected Error Occurred: " + response.status)
            return false;
    }
}

export function getStringHashCode(s) {
  let hash = 0;
  if (s.length === 0) {
    return hash;
  }
  for (let i = 0; i < s.length; i++) {
    const char = s.charCodeAt(i);
    hash = ((hash << 5) - hash) + char;
    hash |= 0; // Ensure the hash remains a 32-bit integer
  }
  return hash;
}

/**
 * Creates a new user in the database. Returns true on success
 * @param {string} username username
 * @param {string} password password (unhashed)
 * @returns {Promise<{
  "username": string,
  "password": number,
  "xp_points": number
}>} indicating success
 */
async function loginUser(username, password) {
    const response = await post('/login', {
        username: username,
        password: getStringHashCode(password)
    });

    if (response === null) return null;

    switch (response.status) {
        case 200:
            return response.json();
        case 409:
            alert("Wrong Username or Password!")
            return null
        case 500:
            alert("Internal Server Error")
            return null;
        default:
            alert("An Unexpected Error Occurred: " + response.status)
            return null;
    }
}

/**
* Update User Information. Returns true on success
* @param {{
  "username": string,
  "password": number,
  "xp_points": number
}} user The user information to update
* @param {number} id The user ID to update
* @returns bool
*/
async function updateUser(user, id) {
    console.log(user.xp_points)

    const response = await post('/updateuser/' + id, user, 'PUT');

    if (response === null) return null;

    switch (response.status) {
        case 200:
            alert("Successfully Update User Information")
            return true;
        case 409:
            alert("Username Already Exist!")
            return false;
        case 400:
            alert("Username Too Long!")
            return false;
        case 500:
            alert("Internal Server Error")
            return false;
        default:
            alert("An Unexpected Error Occurred: " + response.status)
            return false;
    }
}

/**
* Get user by name. Returns user object on success, or null on failure
* @param {string} name the name of the user to retrieve
* @returns {Promise<{
  "user_id": number,
  "username": string,
  "xp_points": number
}>} 
*/
async function getUserByName(name) {
    const response = await get('/users/' + name);

    if (response === null) return null;

    switch (response.status) {
        case 200:
            return response.json();
        case 404:
            alert("User Not Found!")
            return null;
        case 500:
            alert("Internal Server Error")
            return null;
        default:
            alert("An Unexpected Error Occurred: " + response.status)
            return null;
    }
}

/**
* Get user by id. Returns user object on success, or null on failure
* @param {string} id the id of the user to retrieve
* @returns {Promise<{
  "user_id": number,
  "username": string,
  "xp_points": number
}>} 
*/
async function getUserByID(id) {
    const response = await get('/users/id/' + id);

    if (response === null) return null;

    switch (response.status) {
        case 200:
            return response.json();
        case 404:
            alert("User Not Found!")
            return null;
        case 500:
            alert("Internal Server Error")
            return null;
        default:
            alert("An Unexpected Error Occurred: " + response.status)
            return null;
    }
}

/**
* Get leaderboard. Returns leaderboard json on success, or null on failure
* @returns {Promise<{
  "leaderboard": [
    { "username": string, "xp": number }
  ]
}>} 
*/
async function getLeaderboard(){
    const response = await get('/leaderboard');

    if (response === null) return null;

    if (response.ok) return await response.json();

    switch (response.status) {
        case 200:
            return await response.json();
        case 500:
            console.log("internal server error");
            //alert("Internal Server Error")
            return null;
        default:
            //alert("An Unexpected Error Occurred: " + response.status)
            console.error("An Unexpected Error Occurred: " + response.status)
            return null;
    }
}

export {registerUser, loginUser, updateUser, getUserByID, getUserByName, getLeaderboard};