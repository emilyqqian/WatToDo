import React, { useState } from 'react'
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom'
import { Box } from '@mui/material'
import './App.css'
import Home from './Pages/Home.jsx'
import Profile from './Pages/Profile.jsx'
import NavBar from './Components/NavBar.jsx'

const TEST_USER = {
  username: 's969chen',
  userId: 1,
  passwordHash: '676767',
  xp: 0,
}

function App() {
  const [user] = useState(TEST_USER)

  return (
    <Router>
      <Box
        sx={{
          minHeight: '100vh',
          display: 'flex',
          flexDirection: 'column',
          backgroundColor: 'background.default',
        }}
      >
        <NavBar />
        <Box
          component="main"
          sx={{
            flex: 1,
            width: '100%',
            py: 4,
          }}
        >
          <Routes>
            <Route path="/" element={<Home />} />
            <Route path="/profile" element={<Profile user={user} />} />
          </Routes>
        </Box>
      </Box>
    </Router>
  )
}

export default App
